#pragma once

#include <vk_mem_alloc.h>


#include "ImageFormat.h"
#include "ImageProperties.h"
#include "Sampler.h"

#include <glm/glm.hpp>

namespace Veist
{

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

	void transitionImageLayout( VkImageLayout new_layout, VkImageLayout old_layout = VK_IMAGE_LAYOUT_UNDEFINED);
	void generateMipmaps();

	
	~ImageBase();
	ImageBase(const ImageBase&) = default;
	ImageBase& operator=(const ImageBase&) = default;
	
	ImageBase(ImageBase&&);
	ImageBase& operator=(ImageBase&&);

protected:
	
	ImageBase() = default;



	ImageBase(void* data, ImageProperties properties, ImageUsage usage, ImageViewType view_type);
	ImageBase(ImageProperties properties, ImageUsage usage, ImageViewType type);


	//swapchain
	ImageBase(VkImage vk_image, ImageProperties properties, ImageUsage usage, ImageViewType view_type);

	
	
	

	VkImage m_image{ VK_NULL_HANDLE };
	VkImageView m_image_view{ VK_NULL_HANDLE };
	ImageProperties m_properties;
	ImageUsage m_usage{0};
	VmaAllocation m_allocation{nullptr};
};


template<ImageUsage usage, ImageViewType type = ImageViewType::Flat>
class Image : public ImageBase {
public:

	static constexpr bool types_compatible(ImageUsage new_use)
	{
		return ((usage & new_use) == usage);
	}

	//swapchain
	Image(VkImage vk_image, ImageProperties properties) : ImageBase(vk_image, properties, usage, type) {};

	Image(ImageProperties properties) : ImageBase(properties, usage, type) {};
	Image(void* data, ImageProperties properties) : ImageBase(data, properties, usage, type){};
	

	//Move constructor (example use convert StorageCubemap to Cubemap)
	template<ImageUsage use, typename = std::enable_if_t<types_compatible(use)>>
	Image(Image<use, type>&& other)
	{
		ImageBase::operator=(std::move(other));
	}
	
	Image() = default;

};

using Texture = Image<ImageUsage::Texture>;
using Cubemap = Image<ImageUsage::Texture, ImageViewType::Cube>;

using StorageTexture = Image<ImageUsage::Texture | ImageUsage::Storage>;
using StorageCubemap = Image<ImageUsage::Texture | ImageUsage::Storage, ImageViewType::Cube>;

using ColorAttachment = Image<ImageUsage::ColorAttachment>;
using DepthAttachment = Image<ImageUsage::DepthAttachment>;
using ColorTextureAttachment = Image<ImageUsage::ColorAttachment | ImageUsage::Texture>;
using DepthTextureAttachment = Image<ImageUsage::DepthAttachment | ImageUsage::Texture>;
using SwapchainImage = Image<ImageUsage::ColorAttachment | ImageUsage::SwapchainImage>;
using SwapchainDepthAttachment = Image<ImageUsage::DepthAttachment | ImageUsage::SwapchainImage>;

}