
#include "CommandBuffer.h"
#include "CommandPool.h"

#include "Engine/Renderer/RenderModule.h"

CommandBuffer::CommandBuffer(CommandPool* pool, VkFence fence, bool begin_on_creation) : m_command_pool(pool), m_fence(fence)
{
	VkCommandBufferAllocateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	buffer_create_info.pNext = nullptr;
	buffer_create_info.commandPool = m_command_pool->commandPool();
	buffer_create_info.commandBufferCount = 1;
	buffer_create_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VK_CHECK(vkAllocateCommandBuffers(device, &buffer_create_info, &m_cmd_buffer));

	if (begin_on_creation)
	{
		begin();
	}

}



//=================== Command Buffers functions ============================

void CommandBuffer::begin()
{
	VK_CHECK(vkResetCommandBuffer(m_cmd_buffer, 0));
	VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
	cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	cmd_buffer_begin_info.pNext = nullptr;
	cmd_buffer_begin_info.pInheritanceInfo = nullptr;
	cmd_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(m_cmd_buffer, &cmd_buffer_begin_info));
}

void CommandBuffer::end()
{
	//TODO: add check that end renderpass was called

	VK_CHECK(vkEndCommandBuffer(m_cmd_buffer));
}


void CommandBuffer::immediateSubmit()
{
	VK_CHECK(vkEndCommandBuffer(m_cmd_buffer));
	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;
	submit_info.waitSemaphoreCount = 0;
	submit_info.pWaitSemaphores = nullptr;
	submit_info.pWaitDstStageMask = nullptr;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &m_cmd_buffer;
	submit_info.signalSemaphoreCount = 0;
	submit_info.pSignalSemaphores = nullptr;

	//submit command buffer to queue and execute it.
	//fence will block until the commands finish
	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VkQueue graphics_queue = RenderModule::getRenderBackend()->getGraphicsQueue();

	vkResetFences(device, 1, &m_fence);//fence has create signalled flag
	VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit_info, m_fence));


	vkWaitForFences(device, 1, &m_fence, true, 9999999999);
	vkResetFences(device, 1, &m_fence);

	//clear the command pool
	m_command_pool->resetPool();

	CONSOLE_LOG("cmd IMMEDIATE SUBMIT: Data sumbitted to GPU");
}



void CommandBuffer::copyBuffer(const Buffer& src, const Buffer& dst)
{
	VkBufferCopy copy;
	copy.dstOffset = 0;
	copy.srcOffset = 0;

	assert(src.getSize() == dst.getSize());

	copy.size = src.getSize();

	vkCmdCopyBuffer(m_cmd_buffer, src.getBuffer(), dst.getBuffer(), 1, &copy);
}


void CommandBuffer::copyBufferToImage(const Buffer stage_buff, const VkImage image, const std::vector<VkBufferImageCopy>& regions, const ImageProperties& properties)
{
	//prepare pipeline barrier //TODO: Generalize image barrier to other type of images. i think only works with normal textures atm
	VkImageSubresourceRange range;
	range.aspectMask = properties.imageFormat().imageAspectFlags();
	range.baseMipLevel = 0;
	range.levelCount = properties.mipLevels();
	range.baseArrayLayer = 0;
	range.layerCount = properties.layerCount();

	VkImageMemoryBarrier barrier_toTransfer = {};
	barrier_toTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;

	barrier_toTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	barrier_toTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier_toTransfer.image = image;
	barrier_toTransfer.subresourceRange = range;

	barrier_toTransfer.srcAccessMask = 0;
	barrier_toTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

	//barrier the image into the transfer-receive layout
	vkCmdPipelineBarrier(m_cmd_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_toTransfer);

	//copy buffer to image
	vkCmdCopyBufferToImage(m_cmd_buffer, stage_buff.getBuffer(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());


	//barrier to shader readable format
	VkImageMemoryBarrier barrier_toReadable = barrier_toTransfer;

	barrier_toReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier_toReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	barrier_toReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier_toReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	//barrier the image into the shader readable layout
	vkCmdPipelineBarrier(m_cmd_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier_toReadable);

}



void CommandBuffer::dispatch(const ComputePipeline& pipeline, const DescriptorSet& descriptor_set, const glm::u32vec3& group_count)
{

	vkCmdBindPipeline(m_cmd_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline());

	vkCmdBindDescriptorSets(m_cmd_buffer, 
		VK_PIPELINE_BIND_POINT_COMPUTE, 
		pipeline.pipelineLayout(), 
		0, 
		1,
		&descriptor_set.descriptorSet(),
		0, nullptr);

	vkCmdDispatch(m_cmd_buffer, group_count.x, group_count.y, group_count.z);


}



//=================== RenderPass functions =================================

void CommandBuffer::beginRenderPass(const Framebuffer& framebuffer)
{
	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.pNext = nullptr;

	render_pass_begin_info.renderPass = framebuffer.renderpass()->vk_renderpass();
	render_pass_begin_info.renderArea.offset.x = 0;
	render_pass_begin_info.renderArea.offset.y = 0;
	render_pass_begin_info.renderArea.extent = {framebuffer.size().x, framebuffer.size().y};
	render_pass_begin_info.framebuffer = framebuffer.framebuffer();

	std::vector<VkClearValue> clear_values(framebuffer.colorAttachmentCount()); //color + depth buffer
	for (int i = 0; i < framebuffer.colorAttachmentCount(); i++)
	{
		VkClearValue color_clear = {};
		color_clear.color = { {0.005f, 0.005f, 0.005f, 1.0f} };
		clear_values[i] = color_clear;
	}

	{
		VkClearValue depth_clear = {};
		depth_clear.depthStencil.depth = 1.0f; //max depth
		clear_values.push_back(depth_clear);
	}
	
	render_pass_begin_info.clearValueCount = clear_values.size();
	render_pass_begin_info.pClearValues = clear_values.data();


	vkCmdBeginRenderPass(m_cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	
	m_current_renderpass = framebuffer.renderpass();

	setViewport(framebuffer);
	setScissor(glm::u32vec2{ 0, 0 }, framebuffer.size());
}

void CommandBuffer::endRenderPass()
{
	//TODO: add check so that end renderpass cant be called without begin having been called first
	vkCmdEndRenderPass(m_cmd_buffer);
	m_current_renderpass = nullptr;
}


void CommandBuffer::setViewport(const Framebuffer& framebuffer)
{
	
	const VkViewport viewport{0.0f,0.0f, framebuffer.size().x, framebuffer.size().y, 0.0f, 1.0f }; //TODO depth arguments?
	vkCmdSetViewport(m_cmd_buffer, 0, 1, &viewport);

}


void CommandBuffer::setScissor(const glm::u32vec2& offset, const glm::u32vec2& size)
{
	const VkRect2D scissor{ {offset.x, offset.y} , {size.x, size.y}}; 
	vkCmdSetScissor(m_cmd_buffer, 0, 1, &scissor);
}



void CommandBuffer::bindVertexBuffer(const VertexBuffer& vertex_buffer)
{
	VkBuffer buffer = vertex_buffer.getBuffer();
	VkDeviceSize offset = 0;
	vkCmdBindVertexBuffers(m_cmd_buffer, 0, 1, &buffer, &offset);
}


void CommandBuffer::bindIndexBuffer(const IndexBuffer& index_buffer)
{
	VkBuffer buffer = index_buffer.getBuffer();
	VkDeviceSize offset = 0;
	vkCmdBindIndexBuffer(m_cmd_buffer, buffer, 0, VK_INDEX_TYPE_UINT16);
}


void CommandBuffer::drawVertices(uint32_t size)
{
	vkCmdDraw(m_cmd_buffer, size, 1, 0, 0);
}


void CommandBuffer::drawIndexed(uint32_t size)
{
	vkCmdDrawIndexed(m_cmd_buffer, size, 1, 0, 0, 0);
}


void CommandBuffer::bindMaterial(const Material& material)
{
	GraphicsPipeline* pipeline = material.pipeline().get();

	if (pipeline != m_bound_pipeline)
	{
		bindPipeline(*pipeline);
		m_bound_pipeline = pipeline;
	}

	bindDescriptorSet(*pipeline, material.descriptorSet(), 0, nullptr);

}


void CommandBuffer::bindPipeline(GraphicsPipeline& pipeline)
{

	//If pipeline was created for another renderpass either rebuild the pipeline or assert. choose
	if (pipeline.getPipelineRenderpass() != m_current_renderpass->vk_renderpass())
	{
		//CRITICAL_ERROR_LOG("Command Buffer Error: Attempted to bind pipeline for a different renderpass than its own");
		CONSOLE_LOG("Renderpass changed: Pipeline was rebuilt");
		pipeline.rebuildPipeline(*m_current_renderpass);
	}

	VkPipeline vulkan_pipeline = pipeline.pipeline();
	vkCmdBindPipeline(m_cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_pipeline);
	m_bound_pipeline = &pipeline;
}


void CommandBuffer::bindDescriptorSet(const GraphicsPipeline& pipeline, const DescriptorSet& descriptor_set, uint32_t offset_count, uint32_t* p_dynamic_offset)
{

	VkPipelineLayout vulkan_pipeline_layout = pipeline.pipelineLayout();

	vkCmdBindDescriptorSets(
		m_cmd_buffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		vulkan_pipeline_layout,
		descriptor_set.setNumber(),
		1,
		&descriptor_set.descriptorSet(),
		offset_count,
		p_dynamic_offset);
}


void CommandBuffer::setPushConstants(const MatrixPushConstant& push_constant)
{
	VkPipelineLayout vulkan_pipeline_layout = m_bound_pipeline->pipelineLayout();
	vkCmdPushConstants(m_cmd_buffer, vulkan_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(MatrixPushConstant), &push_constant);
}
