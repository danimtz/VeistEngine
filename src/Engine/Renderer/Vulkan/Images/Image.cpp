#include "Image.h"

#include "Engine/Renderer/RenderModule.h"

#include "Engine/Renderer/Vulkan/Buffers/StagingBuffer.h"

static VkImageCreateInfo& getImageCreateInfo(ImageSize size, ImageUsage usage, ImageFormat format, uint32_t mips, uint32_t layers) {
	
	VkImageCreateInfo img_info = {};
	img_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;

	img_info.extent = { size.width, size.height, size.depth };
	img_info.format = VkFormat(format.format());
	img_info.arrayLayers = layers;
	img_info.mipLevels = mips;
	img_info.imageType = VK_IMAGE_TYPE_2D;
	img_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	img_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	img_info.usage = VkImageUsageFlags(usage);
	img_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	img_info.samples = VK_SAMPLE_COUNT_1_BIT;
	return img_info;
}

static VkImageViewCreateInfo& getImageViewCreateInfo(VkImage image, ImageFormat format, uint32_t mips, uint32_t layers) {
	
	VkImageViewCreateInfo img_view_info = {};
	img_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	img_view_info.pNext = nullptr;

	img_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	img_view_info.image = image;
	img_view_info.format = format.format();
	img_view_info.subresourceRange.baseMipLevel = 0;
	img_view_info.subresourceRange.levelCount = mips;
	img_view_info.subresourceRange.baseArrayLayer = 0;
	img_view_info.subresourceRange.layerCount = layers;
	img_view_info.subresourceRange.aspectMask = format.imageAspectFlags();
	return img_view_info;
}

ImageBase::ImageBase(void* data, ImageSize size, ImageUsage usage, ImageFormat format)
{
	usage = usage | ImageUsage::TransferDst;
	//Allocate VkImage
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();
	VkDevice device = RenderModule::getRenderBackend()->getDevice();

	VkImageCreateInfo img_info = getImageCreateInfo(size, usage, format, m_mip_levels, m_layers);

	VmaAllocationCreateInfo img_alloc_info = {};
	img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

	VkImage image;
	VmaAllocation allocation;
	vmaCreateImage(allocator, &img_info, &img_alloc_info, &image, &allocation, nullptr);
	m_image = image;
	m_allocation = allocation;

	RenderModule::getRenderBackend()->pushToDeletionQueue([allocator, image, allocation]() { vmaDestroyImage(allocator, image, allocation); });


	//Create staging buffer and map image data to it
	uint32_t buffer_size = size.width * size.height * size.depth * size.n_channels; //* pixel size?
	StagingBuffer stage_buff = {data, buffer_size};


	//Calculate regions
	std::vector<VkBufferImageCopy> regions;
	regions.reserve(m_layers * m_mip_levels);
	for (uint32_t layer = 0; layer < m_layers; layer++)
	{
		for (uint32_t mip = 0; mip < m_mip_levels; mip++) {
			VkBufferImageCopy copy = {};
			{
				copy.bufferOffset = 0;//data_offset(layer, mip); TODO: fix offset for when mipmaps and layers are implemented
				copy.imageExtent = { size.width, size.height, size.depth };
				copy.imageSubresource.aspectMask = format.imageAspectFlags();//data.format().vk_aspect();
				copy.imageSubresource.mipLevel = mip;
				copy.imageSubresource.baseArrayLayer = layer;
				copy.imageSubresource.layerCount = 1;
			}
			regions.push_back(copy);
		}
	}


	//Sumbit staging buffer copy
	RenderModule::getRenderBackend()->immediateSubmit([&](VkCommandBuffer cmd) {

		//prepare pipeline barrier //TODO: Generalize image barrier to other type of images. i think only works with normal textures atm
		VkImageSubresourceRange range;
		range.aspectMask = format.imageAspectFlags();
		range.baseMipLevel = 0;
		range.levelCount = m_mip_levels;
		range.baseArrayLayer = 0;
		range.layerCount = m_layers;

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


	//Create image view
	VkImageViewCreateInfo view_info = getImageViewCreateInfo(m_image, format, m_mip_levels, m_layers);
	VkImageView image_view;
	vkCreateImageView(device, &view_info, nullptr, &image_view);
	m_image_view = image_view;

	RenderModule::getRenderBackend()->pushToDeletionQueue([=](){ vkDestroyImageView(device, image_view, nullptr);});
}