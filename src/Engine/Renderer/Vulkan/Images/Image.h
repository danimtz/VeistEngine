#pragma once

#include <vk_mem_alloc.h>
#include "Engine/Logger.h"

#include "ImageFormat.h"
#include "ImageProperties.h"

#include <glm/glm.hpp>

enum class ImageUsage : uint32_t
{
	None = 0,
	
	Texture = VK_IMAGE_USAGE_SAMPLED_BIT,
	DepthAttachment = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
	ColorAttachment = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
	Storage = VK_IMAGE_USAGE_STORAGE_BIT,

	SwapchainImage = 0x00000400, //1024 bit flag (custom made not a part of vulkan. used to find layout in renderpass.cpp)

	TransferSrc = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
	TransferDst = VK_IMAGE_USAGE_TRANSFER_DST_BIT,
};
inline constexpr ImageUsage operator|(ImageUsage left, ImageUsage right) {
	return ImageUsage(uint32_t(left) | uint32_t(right));
}
inline constexpr ImageUsage operator|(ImageUsage left, VkImageUsageFlagBits right) {
	return ImageUsage(uint32_t(left) | uint32_t(right));
}

inline constexpr ImageUsage operator&(ImageUsage left, ImageUsage right) {
	return ImageUsage(uint32_t(left) & uint32_t(right));
}
inline constexpr ImageUsage operator&(ImageUsage left, VkImageUsageFlagBits right) {
	return ImageUsage(uint32_t(left) & uint32_t(right));
}

inline constexpr ImageUsage operator~( ImageUsage right) {
	return ImageUsage( ~ uint32_t(right));
}
inline constexpr ImageUsage operator~( VkImageUsageFlagBits right) {
	return ImageUsage( ~ uint32_t(right));
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
	const ImageUsage& imageUsage() const { return m_usage; };

protected:

	//swapchain
	ImageBase(VkImage vk_image, ImageProperties properties, ImageUsage usage, ImageViewType view_type);

	ImageBase(void* data, ImageProperties properties, ImageUsage usage,  ImageViewType view_type);
	ImageBase(ImageProperties properties, ImageUsage usage, ImageViewType type);
	ImageBase() = default;

	VkImage m_image;
	VkImageView m_image_view;
	ImageProperties m_properties;
	ImageUsage m_usage{0};
	VmaAllocation m_allocation;
};


template<ImageUsage usage, ImageViewType type = ImageViewType::Flat>
class Image : public ImageBase {
public:
	
	//swapchain
	Image(VkImage vk_image, ImageProperties properties) : ImageBase(vk_image, properties, usage, type) {};

	Image(ImageProperties properties) : ImageBase(properties, usage, type) {};
	Image(void* data, ImageProperties properties) : ImageBase(data, properties, usage, type){};
	
	Image() = default;

};

using Texture = Image<ImageUsage::Texture>;
using Cubemap = Image<ImageUsage::Texture, ImageViewType::Cube>;
using ColorAttachment = Image<ImageUsage::ColorAttachment>;
using DepthAttachment = Image<ImageUsage::DepthAttachment>;
using SwapchainImage = Image<ImageUsage::ColorAttachment | ImageUsage::SwapchainImage>;
using SwapchainDepthAttachment = Image<ImageUsage::DepthAttachment | ImageUsage::SwapchainImage>;