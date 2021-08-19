#include "Engine/Renderer/Vulkan/Buffers/IndexBuffer.h"
#include "Engine/Renderer/Vulkan/Buffers/StagingBuffer.h"
#include "Engine/Renderer/RenderModule.h"



IndexBuffer::IndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size) : m_index_count(idx_count), m_index_size(index_size),
	m_index_buffer({ idx_count * index_size, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VMA_MEMORY_USAGE_GPU_ONLY } )
{

	//Create staging buffer
	uint32_t size = idx_count * index_size;
	StagingBuffer stage_buff = {indices, size};//deleted in destructor
	
	//sumbit to gpu buffer
	RenderModule::getRenderBackend()->immediateSubmit([&](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = size;
		vkCmdCopyBuffer(cmd, stage_buff.getBuffer(), m_index_buffer.buffer(), 1, &copy);
		});


}
