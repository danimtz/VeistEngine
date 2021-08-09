#include "Engine/Renderer/Vulkan/Buffers/UniformBuffer.h"
#include "Engine/Renderer/RenderModule.h"

UniformBuffer::UniformBuffer(uint32_t size) : m_size(size), m_buffer({ size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU })
{


}


void UniformBuffer::setData(const void* data, uint32_t size, uint32_t offset)
{
	//assert that size and offset is not greater than size of uniform buffer
	if ((offset+size)>m_size) {CRITICAL_ERROR_LOG("Uniform suballocation larger than buffer size!!")}
	
	VmaAllocator allocator = RenderModule::getRenderBackend()->getAllocator();
	
	char* buffer_data;
	vmaMapMemory(allocator, m_buffer.allocation(), (void**)&buffer_data);

	buffer_data += offset;
	memcpy(buffer_data , data, size); //OFFSET WHERE

	vmaUnmapMemory(allocator, m_buffer.allocation());

}