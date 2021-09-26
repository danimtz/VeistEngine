#pragma once

#include <vk_mem_alloc.h>
#include "Engine/Logger.h"

#include "ImageFormat.h"
#include "Engine/Renderer/Vulkan/Images/ImageProperties.h"

#include <glm/glm.hpp>

enum class ImageUsage : uint32_t
{
	None = 0,

	Texture = VK_IMAGE_USAGE_SAMPLED_BIT,
	DepthAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	Storage = VK_IMAGE_USAGE_STORAGE_BIT,

	TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
	TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
};
inline constexpr ImageUsage operator|(ImageUsage left, ImageUsage right) {
	return ImageUsage(uint32_t(left) | uint32_t(right));
}
inline constexpr ImageUsage operator|(ImageUsage left, VkImageUsageFlagBits right) {
	return ImageUsage(uint32_t(left) | uint32_t(right));
}

enum class ImageViewType : uint32_t {
	Flat = VK_IMAGE_VIEW_TYPE_2D,
	Cube = VK_IMAGE_VIEW_TYPE_CUBE
};


class ImageBase
{
public:

	VkImage image() const { return m_image; };
	VkImageView imageView() const { return m_image_view; };
	const ImageProperties& properties() const {return m_properties;};

protected:

	ImageBase(void* data, ImageProperties properties, ImageUsage usage,  ImageViewType type);


	VkImage m_image;
	VkImageView m_image_view;
	VmaAllocation m_allocation;
	ImageProperties m_properties;
};


template<ImageUsage usage, ImageViewType type = ImageViewType::Flat>
class Image : public ImageBase {
public:
	
	Image(void* data, ImageProperties properties) : ImageBase(data, properties, usage, type){};
	
};

using Texture = Image<ImageUsage::Texture>;
using Cubemap = Image<ImageUsage::Texture, ImageViewType::Cube>;