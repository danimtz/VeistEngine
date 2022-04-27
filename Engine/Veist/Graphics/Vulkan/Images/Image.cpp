#include "pch.h"
#include "Image.h"

#include "Veist/Graphics/RenderModule.h"

#include "Veist/Graphics/Vulkan/Buffers/StagingBuffer.h"





namespace Veist
{




	static VkImageCreateInfo getImageCreateInfo( ImageProperties properties, ImageUsage usage, ImageViewType view_type, std::vector<uint32_t>& indices) {
	
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
		img_info.sharingMode = VK_SHARING_MODE_CONCURRENT;

	
		img_info.pQueueFamilyIndices = indices.data();
		img_info.queueFamilyIndexCount = indices.size();
		img_info.samples = VK_SAMPLE_COUNT_1_BIT;

		img_info.flags = (view_type == ImageViewType::Cube) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

		return img_info;
	}


	static VkImageViewCreateInfo getImageViewCreateInfo(VkImage image, ImageProperties& properties, ImageViewType view_type) {
	
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
		uint32_t mip_levels = 1; //m_properties.mipLevels(); //TODO add support for build in mipmaps
		
		//Create staging buffer and map image data to it
		uint32_t buffer_size = m_properties.layerSizeBytesNoMips() * m_properties.layerCount(); //* pixel size?
		StagingBuffer stage_buff = { data, buffer_size };


		//Calculate regions
		std::vector<VkBufferImageCopy> regions;
		regions.reserve(m_properties.layerCount() * mip_levels);
		for (uint32_t layer = 0; layer < m_properties.layerCount(); layer++)
		{
			for (uint32_t mip = 0; mip < mip_levels; mip++) {
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
		auto cmd_buffer = RenderModule::getBackend()->createDisposableCmdBuffer();
		cmd_buffer.copyBufferToImage(stage_buff, this, regions, m_properties);
		cmd_buffer.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());

		if (m_properties.mipLevels() > 1)
		{
			transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			generateMipmaps();
		}
			
		

		

	
	}





	ImageBase::ImageBase(ImageProperties properties, ImageUsage usage, ImageViewType view_type) : m_properties(properties), m_usage(usage)
	{
	

		usage = usage | ImageUsage::TransferDst;
		if (m_properties.mipLevels() > 1)
		{
			usage = usage | ImageUsage::TransferSrc; //If it has mipmaps
		}


		//Allocate VkImage
		VmaAllocator allocator = RenderModule::getBackend()->getAllocator();
		VkDevice device = RenderModule::getBackend()->getDevice();

		std::vector<uint32_t> queue_indices;
		queue_indices.push_back(RenderModule::getBackend()->getGraphicsFamily());
		queue_indices.push_back(RenderModule::getBackend()->getTransferFamily());
		VkImageCreateInfo img_info = getImageCreateInfo(m_properties, usage, view_type, queue_indices);

		VmaAllocationCreateInfo img_alloc_info = {};
		img_alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkImage image;
		VmaAllocation allocation;
		vmaCreateImage(allocator, &img_info, &img_alloc_info, &image, &allocation, nullptr);
		m_image = image;
		m_allocation = allocation;

		//RenderModule::getBackend()->pushToDeletionQueue([allocator, image, allocation]() { vmaDestroyImage(allocator, image, allocation); });



		//Create image view
		VkImageViewCreateInfo view_info = getImageViewCreateInfo(m_image, m_properties, view_type);

		vkCreateImageView(device, &view_info, nullptr, &m_image_view);
		VkImageView image_view = m_image_view;


	
	//TODO is this transition here necessary
		//If storage image set layout to general
		if ((usage & ImageUsage::Storage) == ImageUsage::Storage)
		{
			transitionImageLayout(VK_IMAGE_LAYOUT_GENERAL);
		}
		else if((usage & ImageUsage::SwapchainImage) != ImageUsage::SwapchainImage)
		{
			//transitionImageLayout(getImageLayout(usage));
		}

	}


	ImageBase::ImageBase(VkImage vk_image, ImageProperties properties, ImageUsage usage, ImageViewType view_type) : m_image(vk_image), m_properties(properties), m_usage(usage)
	{

		//Create image view
		VkDevice device = RenderModule::getBackend()->getDevice();

		VkImageViewCreateInfo view_info = getImageViewCreateInfo(m_image, m_properties, view_type);
		VkImageView image_view;
		vkCreateImageView(device, &view_info, nullptr, &image_view);
		m_image_view = image_view;


	}




	void ImageBase::transitionImageLayout(VkImageLayout new_layout, VkImageLayout old_layout)
	{
		CommandBuffer cmd = RenderModule::getBackend()->createDisposableCmdBuffer();

		cmd.pipelineBarrier({ImageBarrier::createTransitionBarrier(this, old_layout, new_layout)});
		//TODO: move barrier functionality to CommandBuffer class
		 //from vulkan-tutorial

/*
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		barrier.image = m_image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = m_properties.mipLevels();
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = m_properties.layerCount();
		barrier.subresourceRange.aspectMask = m_properties.imageFormat().imageAspectFlags();

		barrier.srcAccessMask = 0; // TODO
		barrier.dstAccessMask = 0; // TODO

		vkCmdPipelineBarrier( cmd.vk_commandBuffer(), //Non blocking barrier for now
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT , 
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
*/
		cmd.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());
	}


	void ImageBase::generateMipmaps() //https://vulkan-tutorial.com/Generating_Mipmaps
	{
		if (!(m_properties.mipLevels() > 1))
		{
			CONSOLE_LOG("Warning: Generate mipmaps called on an image with no mip levels")
			return;
		}
		CommandBuffer cmd = RenderModule::getBackend()->createDisposableCmdBuffer();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = m_properties.layerCount();
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.aspectMask = m_properties.imageFormat().imageAspectFlags();

		int32_t mip_width = m_properties.imageSize().width;
		int32_t mip_height = m_properties.imageSize().height;

		for (uint32_t i = 1; i < m_properties.mipLevels(); i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(cmd.vk_commandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mip_width, mip_height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = m_properties.layerCount();
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = m_properties.layerCount();

			vkCmdBlitImage(cmd.vk_commandBuffer(),
				m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(cmd.vk_commandBuffer(),
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mip_width > 1) mip_width /= 2;
			if (mip_height > 1) mip_height /= 2;
		}

		barrier.subresourceRange.baseMipLevel = m_properties.mipLevels() - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(cmd.vk_commandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		cmd.immediateSubmit(RenderModule::getBackend()->getGraphicsQueue());
	}



	//Destructor
	ImageBase::~ImageBase()
	{

		VkDevice device = RenderModule::getBackend()->getDevice();

		if (m_image_view != VK_NULL_HANDLE)
		{
			vkDestroyImageView(device, m_image_view, nullptr);

		}
		if (m_allocation != nullptr)
		{
			VmaAllocator allocator = RenderModule::getBackend()->getAllocator();
			vmaDestroyImage(allocator, m_image, m_allocation);
		}


	}



	//Move assignment
	ImageBase::ImageBase(ImageBase&& other)
	{
		m_image = other.m_image;
		m_image_view = other.m_image_view;
		m_properties = other.m_properties;
		m_usage = other.m_usage;
		m_allocation = other.m_allocation;

		other.m_image = VK_NULL_HANDLE;
		other.m_image_view = VK_NULL_HANDLE;
		other.m_allocation = nullptr;
	}



	//Move copy
	ImageBase& ImageBase::operator=(ImageBase&& other)
	{
		if (this != &other)
		{
			// Free the existing resource if it exists
			VkDevice device = RenderModule::getBackend()->getDevice();
			if (m_image_view != VK_NULL_HANDLE)
			{

				vkDestroyImageView(device, m_image_view, nullptr);
			}
			if (m_allocation != nullptr)
			{
				VmaAllocator allocator = RenderModule::getBackend()->getAllocator();
				vmaDestroyImage(allocator, m_image, m_allocation);
			}
	

			//copy resources
			m_image = other.m_image;
			m_image_view = other.m_image_view;
			m_properties = other.m_properties;
			m_usage = other.m_usage;
			m_allocation = other.m_allocation;

			other.m_image = VK_NULL_HANDLE;
			other.m_image_view = VK_NULL_HANDLE;
			other.m_allocation = nullptr;
		}
		return *this;

	}


}