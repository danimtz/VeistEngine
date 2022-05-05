#include "pch.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"
#include "Veist/Graphics/RenderModule.h"


namespace Veist
{


	ShaderBuffer::ShaderBuffer(uint32_t subbuffer_size, uint32_t subbuffer_count, ShaderBufferUsage usage) : m_usage(usage), m_range(subbuffer_size), m_subbuffer_count(subbuffer_count)
	{
		uint32_t size = subbuffer_size;
		uint32_t minUBOalignment = RenderModule::getBackend()->getGPUinfo().properties.limits.minUniformBufferOffsetAlignment;
	
		if (minUBOalignment > 0) {
			size = (size + minUBOalignment - 1) & ~(minUBOalignment - 1);
		}

		m_size = size * subbuffer_count;
		m_buffer = { m_size, static_cast<VkBufferUsageFlagBits>(usage), VMA_MEMORY_USAGE_CPU_TO_GPU };
		m_offset = size;
	}





	ShaderBuffer::ShaderBuffer(uint32_t size, ShaderBufferUsage usage) : m_size(size), m_usage(usage),
		m_buffer({ size, static_cast<VkBufferUsageFlagBits>(usage), VMA_MEMORY_USAGE_CPU_TO_GPU })
	{

	}




	void ShaderBuffer::setData(const void* data, uint32_t size, uint32_t sub_allocation_num)
	{
		//assert that size and offset is not greater than size of uniform buffer
		if (((sub_allocation_num*m_offset)+size)>m_size) {CRITICAL_ERROR_LOG("Uniform suballocation larger than buffer size!!")}
	
		VmaAllocator allocator = RenderModule::getBackend()->getAllocator();
	
		char* buffer_data;
		vmaMapMemory(allocator, m_buffer.allocation(), (void**)&buffer_data);

		buffer_data += sub_allocation_num*m_offset;
		memcpy(buffer_data , data, size); //OFFSET WHERE

		vmaUnmapMemory(allocator, m_buffer.allocation());

	}

}