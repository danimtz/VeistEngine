#pragma once


#include "vulkan/vulkan.h"
#include "Veist/Renderer/Vulkan/Buffers/Buffer.h"



namespace Veist
{



class IndexBuffer : public Buffer
{
public:

	IndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size);

	uint32_t getIndexCount() const { return m_index_count; };

private:

	uint32_t			m_index_count;
	uint32_t			m_indices_size; //VK_INDEX_TYPE_UINT16
};

}