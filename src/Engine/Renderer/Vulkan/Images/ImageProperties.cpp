
#include "ImageProperties.h"


glm::u32vec3 ImageProperties::mipSize(uint32_t mip_level) 
{
	
	uint32_t width  = std::max<uint32_t>(1, m_size.width  >> mip_level);
	uint32_t height = std::max<uint32_t>(1, m_size.height >> mip_level);
	uint32_t depth  = std::max<uint32_t>(1, m_size.depth  >> mip_level);

	return {width, height, depth};
}


size_t ImageProperties::bufferOffset(uint32_t layer, uint32_t mip_level) 
{
	uint32_t offset = 0;
	 
	if (layer > 0)
	{
		//offset = layer * layerSizeBytes();TODO
	}

	for (uint32_t i = 0; i < mip_level; i++)
	{
		//offset += mipLevelBytes(i);TODO
	}
	
	return offset;

}


uint32_t ImageProperties::sizeInPixels()
{


	return m_size.width * m_size.height * m_size.depth * m_size.n_channels;

}
