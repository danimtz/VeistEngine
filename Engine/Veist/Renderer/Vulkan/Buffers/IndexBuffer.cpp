#include "pch.h"


#include "Veist/Renderer/Vulkan/Buffers/IndexBuffer.h"
#include "Veist/Renderer/Vulkan/Buffers/StagingBuffer.h"
#include "Veist/Renderer/RenderModule.h"


namespace Veist
{


IndexBuffer::IndexBuffer(void* indices, uint32_t idx_count, uint32_t index_size) : m_index_count(idx_count), m_indices_size(index_size),
	Buffer({ idx_count * index_size, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT), VMA_MEMORY_USAGE_GPU_ONLY } )
{

	//Create staging buffer
	StagingBuffer stage_buff = {indices, m_size };//deleted in destructor
	
	//sumbit to gpu buffer

	auto cmd_buffer = RenderModule::getRenderBackend()->createDisposableCmdBuffer();
	cmd_buffer.copyBuffer(stage_buff, *this);
	cmd_buffer.immediateSubmit();
	/*RenderModule::getRenderBackend()->immediateSubmit([&](VkCommandBuffer cmd) {
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = size;
		vkCmdCopyBuffer(cmd, stage_buff.getBuffer(), m_index_buffer.buffer(), 1, &copy);
		});
		*/
}

}