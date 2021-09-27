
#include "ImageProperties.h"


glm::u32vec3 ImageProperties::mipSize(uint32_t mip_level) 
{
	
	uint32_t width  = std::max<uint32_t>(1, m_size.width  >> mip_level);
	uint32_t height = std::max<uint32_t>(1, m_size.height >> mip_level);
	uint32_t depth  = std::max<uint32_t>(1, m_size.depth  >> mip_level);

	return {width, height, depth};
}

uint32_t ImageProperties::mipLevelBytes(uint32_t mip_level)
{
	uint32_t mip_bytes = 0;
	auto mip_size = ImageProperties::mipSize(mip_level);

	mip_bytes = mip_size.x * mip_size.y * mip_size.z * 4/*TODO: *bytes_per_pixel*/;

	return mip_bytes;
}

size_t ImageProperties::bufferOffset(uint32_t layer, uint32_t mip_level) 
{
	uint32_t offset = 0;
	 
	if (layer > 0)
	{
		offset = layer * layerSizeBytes();
	}

	for (uint32_t i = 0; i < mip_level; i++)
	{
		offset += mipLevelBytes(i);
	}
	
	return offset;

}

uint32_t ImageProperties::layerSizeBytes()
{
	uint32_t layer_size = 0;
	for (uint32_t i = 0; i < m_mip_levels; i++) {
		layer_size += mipLevelBytes(i);
	}
	return layer_size;
}

uint32_t ImageProperties::sizeInPixels()
{


	return m_size.width * m_size.height * m_size.depth * m_size.n_channels;

}
