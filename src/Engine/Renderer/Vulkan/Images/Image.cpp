#include "Image.h"

#include "Engine/Renderer/RenderModule.h"

#include "Engine/Renderer/Vulkan/Buffers/StagingBuffer.h"

static VkImageCreateInfo& getImageCreateInfo( ImageProperties properties, ImageUsage usage, ImageViewType view_type) {
	
	VkImageCreateInfo img_info = {};
	img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	img_info.extent = { properties.imageSize().width, properties.imageSize().height, properties.imageSize().depth };
	img_info.format = VkFormat(properties.imageFormat().format());
	img_info.arrayLayers = properties.layerCount();
	img_info.mipLevels = properties.mipLevels();
	img_info.imageType = VK_IMAGE_TYPE_2D;
	img_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	img_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	img_info.usage = VkImageUsageFlags(usage);
	img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	img_info.samples = VK_SAMPLE_COUNT_1_BIT;

	img_info.flags = (view_type == ImageViewType::Cube) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

	return img_info;
}

static VkImageViewCreateInfo& getImageViewCreateInfo(VkImage image, ImageProperties properties, ImageViewType view_type) {
	
	VkImageViewCreateInfo img_view_info = {};
	img_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	img_view_info.pNext = nullptr;

	img_view_info.viewType = VkImageViewType(view_type);
	img_view_info.image = image;
	img_view_info.format = VkFormat(properties.imageFormat().format());
	img_view_info.subresourceRange.baseMipLevel = 0;
	img_view_info.subresourceRange.levelCount = properties.mipLevels();
	img_view_info.subresourceRange.baseArrayLayer = 0;
	img_view_info.subresourceRange.layerCount = properties.layerCount();
	img_view_info.subresourceRange.aspectMask = properties.imageFormat().imageAspectFlags();
	
	return img_view_info;
}


ImageBase::ImageBase(void* data, ImageProperties properties, ImageUsage usage, ImageViewType view_type) : ImageBase(properties, usage, view_type)
{

	//Create staging buffer and map image data to it
	uint32_t buffer_size = m_properties.layerSizeBytes() * m_properties.layerCount(); //* pixel size?
	StagingBuffer stage_buff = { data, buffer_size };


	//Calculate regions
	std::vector<VkBufferImageCopy> regions;
	regions.reserve(m_properties.layerCount() * m_properties.mipLevels());
	for (uint32_t layer = 0; layer < m_properties.layerCount(); layer++)
	{
		for (uint32_t mip = 0; mip < m_properties.mipLevels(); mip++) {
			VkBufferImageCopy copy = {};
			{
				copy.bufferOffset = m_properties.bufferOffset(layer, mip);//data_offset(layer, mip); TODO: fix offset for when mipmaps and layers are implemented
				copy.imageExtent = { m_properties.imageSize().width, m_properties.imageSize().height, m_properties.imageSize().depth };
				copy.imageSubresource.aspectMask = m_properties.imageFormat().imageAspectFlags();
				copy.imageSubresource.mipLevel = mip;
				copy.imageSubresource.baseArrayLayer = layer;
				copy.imageSubresource.layerCount = 1;
			}
			regions.push_back(copy);
		}
	}


	//Sumbit staging buffer copy TODO: change to use cmd buffer
	RenderModule::getRenderBackend()->immediateSubmit([&](VkCommandBuffer cmd) {

		//prepare pipeline barrier //TODO: Generalize image barrier to other type of images. i think only works with normal textures atm
		VkImageSubresourceRange range;
		range.aspectMask = m_properties.imageFormat().imageAspectFlags();
		range.baseMipLevel = 0;
		range.levelCount = m_properties.mipLevels();
		range.baseArrayLayer = 0;
		range.layerCount = m_properties.layerCount();

		VkImageMemoryBarrier barrier_toTransfer = {};
		barrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

		barrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier_toTransfer.image = m_image;
		barrier_toTransfer.subresourceRange = range;

		barrier_toTransfer.srcAccessMask = 0;
		barrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		//barrier the image into the transfer-receive layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_toTransfer);

		//copy buffer to image
		vkCmdCopyBufferToImage(cmd, stage_buff.getBuffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());


		//barrier to shader readable format
		VkImageMemoryBarrier barrier_toReadable = barrier_toTransfer;

		barrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		barrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//barrier the image into the shader readable layout
		vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_toReadable);

		});


	
}

ImageBase::ImageBase(ImageProperties properties, ImageUsage usage, ImageViewType view_type) : m_properties(properties), m_usage(usage)
{
	


	usage = usage | ImageUsage::TransferDst;
	

	//Allocate VkImage
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();
	VkDevice device = RenderModule::getRenderBackend()->getDevice();

	VkImageCreateInfo img_info = getImageCreateInfo(m_properties, usage, view_type);

	VmaAllocationCreateInfo img_alloc_info = {};
	img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VkImage image;
	VmaAllocation allocation;
	vmaCreateImage(allocator, &img_info, &img_alloc_info, &image, &allocation, nullptr);
	m_image = image;
	m_allocation = allocation;

	RenderModule::getRenderBackend()->pushToDeletionQueue([allocator, image, allocation]() { vmaDestroyImage(allocator, image, allocation); });



	//Create image view
	VkImageViewCreateInfo view_info = getImageViewCreateInfo(m_image, m_properties, view_type);
	VkImageView image_view;
	vkCreateImageView(device, &view_info, nullptr, &image_view);
	m_image_view = image_view;


	//If image view is being used in swapchain use swapchain deletion queue
	if ((usage & ImageUsage::SwapchainImage) != ImageUsage::None)
	{
		RenderModule::getRenderBackend()->pushToSwapchainDeletionQueue([=]() { vkDestroyImageView(device, image_view, nullptr);	});
	}
	else 
	{
		RenderModule::getRenderBackend()->pushToDeletionQueue([=]() { vkDestroyImageView(device, image_view, nullptr); });
	}
	

}


ImageBase::ImageBase(VkImage vk_image, ImageProperties properties, ImageUsage usage, ImageViewType view_type) : m_image(vk_image), m_properties(properties), m_usage(usage)
{

	//Create image view
	VkDevice device = RenderModule::getRenderBackend()->getDevice();

	VkImageViewCreateInfo view_info = getImageViewCreateInfo(m_image, m_properties, view_type);
	VkImageView image_view;
	vkCreateImageView(device, &view_info, nullptr, &image_view);
	m_image_view = image_view;

	RenderModule::getRenderBackend()->pushToDeletionQueue([=]() { vkDestroyImageView(device, image_view, nullptr); });

}