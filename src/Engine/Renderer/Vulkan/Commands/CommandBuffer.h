#pragma once



#include <vulkan/vulkan.h>


#include "Engine/Renderer/Vulkan/Framebuffers/Framebuffer.h"
#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"
#include "Engine/Renderer/Vulkan/Buffers/IndexBuffer.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Material/Material.h"

class CommandPool;

//class Framebuffer;
//class VertexBuffer;
//class IndexBuffer;
//class Material;

class CommandBuffer
{
public:

	CommandBuffer() = default;

	VkCommandBuffer vk_commandBuffer() const { return m_cmd_buffer; };
	const VkFence& fence() const { return m_fence; }

	

//Command buffer recording functions
	void begin();
	void end();
	void immediateSubmit();
	void copyBuffer(const Buffer& src, const Buffer& dst);
	void copyBufferToImage(const Buffer stage_buff, const VkImage image, const std::vector<VkBufferImageCopy>& regions,  const ImageProperties& properties );
	//void pipelineBarrier(); TODO: Pipeline barriers inside copyBufferToImage, extract them into functions

	void dispatch(const ComputePipeline& pipeline, const DescriptorSet& descriptor_set, const glm::u32vec3& group_count);//TODO: only 1 descriptor set for now for compute shaders and no push constants
	
	void calcSizeAndDispatch(const ComputePipeline& pipeline, const DescriptorSet& descriptor_set, const ImageSize& size);
	void calcSizeAndDispatch(const ComputePipeline& pipeline, const DescriptorSet& descriptor_set, const glm::u32vec3& size);

//Renderpass recording functions
	void beginRenderPass(const Framebuffer& framebuffer);
	void endRenderPass();

	void setViewport(const Framebuffer& framebuffer);
	void setScissor(const glm::u32vec2& offset, const glm::u32vec2& size);

	void bindVertexBuffer(const VertexBuffer& vertex_buffer);
	void bindIndexBuffer(const IndexBuffer& index_buffer);
	void drawVertices(uint32_t size);
	void drawIndexed(uint32_t size);
	void bindMaterial(const Material& material);

	void bindPipeline(GraphicsPipeline& pipeline);//maybe combine bindPipeline and bindDescriptorSet

	//binds descriptor sets one at a time. TODO: make it bind all sets at once maybe? feed it a vector of VkDescriptorSet
	void bindDescriptorSet(const GraphicsPipeline& pipeline, const DescriptorSet& descriptor_set, uint32_t offset_count, uint32_t* p_dynamic_offset);

	//TODO: rework this one
	void setPushConstants(const MatrixPushConstant& push_constant);


private:
	
	friend class CommandPool;
	CommandBuffer(CommandPool* pool, VkFence fence, bool begin_on_creation = false);
	
	CommandPool* m_command_pool; 
	VkCommandBuffer m_cmd_buffer;

	VkFence m_fence;

	const GraphicsPipeline* m_bound_pipeline{nullptr};
	const RenderPass* m_current_renderpass{ nullptr };
};

