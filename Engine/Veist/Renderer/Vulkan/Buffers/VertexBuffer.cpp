#include "pch.h"

#include "Veist/Renderer/Vulkan/Buffers/VertexBuffer.h"
#include "Veist/Renderer/Vulkan/Buffers/StagingBuffer.h"
#include "Veist/Renderer/RenderModule.h"


namespace Veist
{


VertexBuffer::VertexBuffer(void* vertices, uint32_t size) :  
	Buffer( size, static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT) , VMA_MEMORY_USAGE_GPU_ONLY) 
{

	StagingBuffer stage_buff = { vertices, size };//deleted in destructor
	//sumbit to gpu buffer
	auto cmd_buffer = RenderModule::getBackend()->createDisposableCmdBuffer();
	cmd_buffer.copyBuffer(stage_buff, *this);
	cmd_buffer.immediateSubmit();

	/*RenderModule::getBackend()->immediateSubmit([&](VkCommandBuffer cmd){
		VkBufferCopy copy;
		copy.dstOffset = 0;
		copy.srcOffset = 0;
		copy.size = size;
		vkCmdCopyBuffer(cmd, stage_buff.getBuffer(), m_buffer.buffer(), 1, &copy);
	});*/

	
}

}