#pragma once

#include <vulkan/vulkan.h>
//#include <fstream>
//#include <vector>
//#include <string>
//#include "Logger.h"
#include "Veist/Graphics/Vulkan/ShaderAndPipelines/Shader.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"
#include "Veist/Graphics/Vulkan/Framebuffers/Renderpass.h"

#include <glm/glm.hpp>

namespace Veist
{

//TODO This definately needs to go somewhere else in the future.(shader class? material class?)
struct MatrixPushConstant//Very specific and temporary name, so that i know to change it later on
{
	glm::mat4 mat1;//matrix1
	glm::mat4 mat2;//matrix2
};


enum class DepthTest {
	None = 0,
	Read = 1,
	Write = 2,
	ReadWrite = Read | Write
};

enum class PolygonMode
{
	Fill = 0,
	Line = 1,
	Point = 2
};

enum class CullMode
{
	None = 0,
	Front = 1,
	Back = 2
};
class GraphicsPipeline
{
public:

	GraphicsPipeline(VkPipeline pipeline, VkRenderPass renderpass, VkPipelineLayout pipeline_layout) : m_pipeline(pipeline), m_renderpass(renderpass), m_pipeline_layout(pipeline_layout){}
	~GraphicsPipeline() {};

	VkPipeline pipeline() const { return m_pipeline; };
	VkRenderPass pipelineRenderpass() const { return m_renderpass; };
	VkPipelineLayout pipelineLayout() const { return m_pipeline_layout;}
	//Change pipeline renderpass by rebuilding it
	//void rebuildPipeline (const RenderPass& renderpass);

private:

	VkPipeline	m_pipeline;
	VkRenderPass m_renderpass;
	VkPipelineLayout m_pipeline_layout;

};

struct MaterialSettings;

class GraphicsPipelineBuilder
{
public:

	//Create graphics pipeline builder, but dont build the pipeline yet. lazy initialization of vkPipeline
	GraphicsPipelineBuilder(const MaterialSettings& settings, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE);

	
	GraphicsPipeline buildPipeline(const RenderPass* renderpass);
	VkPipelineLayout pipelineLayout() const { return m_pipeline_layout; };
	ShaderProgram* shaderProgram() const { return m_shader_program.get(); };
	
private:

	void createShaderProgram(const MaterialSettings& settings);
	void setVertexInputDescriptions(const VertexDescription& vertex_desc);
	void createPipelineStates(uint32_t attachment_count);
	void createPipelineLayout();


	//Constructor parameters (MORE TO BE ADDED)
	VkPrimitiveTopology m_topology;
	VkPolygonMode		m_polygon_mode;
	VkCullModeFlags		m_cull_mode;
	VkFrontFace			m_front_face;
	DepthTest			m_depth_test;

	//Pipeline attributes
	
	
	std::shared_ptr<ShaderProgram>			m_shader_program;

	//VkRenderPass m_renderpass;

	VkPipelineVertexInputStateCreateInfo			m_vertex_input_info = {};
	VkPipelineInputAssemblyStateCreateInfo			m_input_assembly_info = {};
	VkPipelineRasterizationStateCreateInfo			m_rasterizer_info = {};
	VkPipelineMultisampleStateCreateInfo			m_multisample_state_info = {};
	std::vector<VkPipelineColorBlendAttachmentState> m_color_blend_attachment_state = {};
	VkPipelineColorBlendStateCreateInfo				m_color_blend_state_info = {};
	VkPipelineDepthStencilStateCreateInfo			m_depth_stencil_state_info = {};
	VkPipelineViewportStateCreateInfo				m_viewport_state_info = {};
	VkPipelineDynamicStateCreateInfo				m_dynamic_state_info = {};

	std::vector<VkDynamicState> m_dynamic_states;

	std::vector<VkVertexInputBindingDescription>	m_vertex_bindings;
	std::vector<VkVertexInputAttributeDescription>	m_vertex_attributes;

	VkPipelineLayout								m_pipeline_layout;


	friend class GraphicsPipeline;

};









}