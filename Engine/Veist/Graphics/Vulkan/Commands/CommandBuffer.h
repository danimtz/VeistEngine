#pragma once



#include <vulkan/vulkan.h>


#include "Veist/Graphics/Vulkan/Framebuffers/Framebuffer.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexBuffer.h"
#include "Veist/Graphics/Vulkan/Buffers/IndexBuffer.h"
#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Barriers/Barrier.h"
//#include "Veist/Resources/EngineResources.h"
#include "Veist/Material/Material.h"


namespace Veist
{

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
	const RenderPass* currentRenderPass() const {return m_current_renderpass; };
	const VkFence& fence() const { return m_fence; }

	

//Command buffer recording functions
	void begin();
	void end();
	void immediateSubmit(VkQueue queue);
	void copyBuffer(const Buffer& src, const Buffer& dst);
	void copyBufferToImage(const Buffer& stage_buff, const ImageBase* image, const std::vector<VkBufferImageCopy>& regions,  const ImageProperties& properties );
	
	//TODO: Pipeline barriers inside copyBufferToImage, extract them into functions

	void pipelineBarrier(const std::vector<ImageBarrier>& image_barriers);
	void pipelineBarrier(const std::vector<BufferBarrier>& buffer_barriers);
	void pipelineBarrier(const std::vector<ImageBarrier>& image_barriers, const std::vector<BufferBarrier>& buffer_barriers); 



	void dispatch(const ComputePipeline& pipeline, const DescriptorSet& descriptor_set, const glm::u32vec3& group_count);//TODO: only 1 descriptor set for now for compute shaders and no push constants
	
	void setComputePushConstant(const ComputePipeline& pipeline, float constant_value); //TODO: Should be a class PushConstant, not a float etc etc.

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
	void bindMaterialType(uint32_t type);
	void bindMaterial(Material& material);

	void bindPipeline(GraphicsPipeline& pipeline);//maybe combine bindPipeline and bindDescriptorSet

	//binds descriptor sets one at a time. TODO: make it bind all sets at once maybe? feed it a vector of VkDescriptorSet
	void bindDescriptorSet(const DescriptorSet& descriptor_set, uint32_t offset_count = 0, uint32_t* p_dynamic_offset = nullptr);

	//TODO: rework this one
	void setPushConstants(const MatrixPushConstant& push_constant);


private:
	
	friend class CommandPool;
	CommandBuffer(CommandPool* pool, VkFence fence, bool begin_on_creation = false);
	
	CommandPool* m_command_pool; 
	VkCommandBuffer m_cmd_buffer;

	VkFence m_fence;

	const GraphicsPipeline* m_bound_pipeline{ nullptr };
	const RenderPass* m_current_renderpass{ nullptr };
};

}