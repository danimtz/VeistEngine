#pragma once

#include <vk_mem_alloc.h>
#include "Engine/Logger.h"

#include "ImageFormat.h"

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





struct ImageSize {
	ImageSize(uint32_t w, uint32_t h, uint32_t n_c) : width(w), height(h), depth(1), n_channels(n_c){};
	ImageSize(uint32_t w, uint32_t h, uint32_t d, uint32_t n_c) : width(w), height(h), depth(d), n_channels(n_c) {};
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t n_channels;
};

class Image
{
public:

	Image(void* data, ImageSize size, ImageUsage usage, ImageFormat format = {VK_FORMAT_R8G8B8A8_SRGB});


private:
	
	uint32_t m_mip_levels = 1;
	uint32_t m_layers = 1;

	VkImage m_image;
	VkImageView m_image_view;
	VmaAllocation m_allocation;

};

