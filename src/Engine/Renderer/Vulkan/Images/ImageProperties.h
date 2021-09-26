#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include "Engine/Renderer/Vulkan/Images/ImageFormat.h"







struct ImageSize {
	ImageSize(uint32_t w, uint32_t h, uint32_t n_c) : width(w), height(h), depth(1), n_channels(n_c) {};
	ImageSize(uint32_t w, uint32_t h, uint32_t d, uint32_t n_c) : width(w), height(h), depth(d), n_channels(n_c) {};
	uint32_t width;
	uint32_t height;
	uint32_t depth;
	uint32_t n_channels;
};





class ImageProperties
{
public:

	ImageProperties(ImageSize size, ImageFormat format, uint32_t mip_levels, uint32_t layers) : m_size(size), m_format(format), m_mip_levels(mip_levels), m_layers(layers) {};


	const ImageSize& imageSize() const { return m_size; };
	const ImageFormat& format() const { return m_format; };
	uint32_t mipLevels() const { return m_mip_levels; };
	uint32_t layerCount() const { return m_layers; };
	
	size_t bufferOffset(uint32_t layer = 0, uint32_t mip_level = 0);

	uint32_t sizeInPixels();

private:

	ImageSize m_size;
	ImageFormat m_format;
	uint32_t m_mip_levels;
	uint32_t m_layers;


	glm::u32vec3 mipSize(uint32_t mip_level);

};

