#pragma once

#include <vulkan/vulkan.h>
//#include <fstream>
//#include <vector>
//#include <string>
//#include "Logger.h"
#include "Veist/Renderer/Vulkan/ShaderAndPipelines/Shader.h"
#include "Veist/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Veist/Renderer/Vulkan/Framebuffers/Renderpass.h"

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




class GraphicsPipelineBuilder
{
public:


	GraphicsPipelineBuilder(const std::string& shader_name, const VertexDescription& vertex_desc, DepthTest depth_test, VkPrimitiveTopology topology,
		VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face); 
	
	
	VkPipeline buildPipeline(const RenderPass& renderpass);

	VkRenderPass vk_renderpass() const {return m_renderpass;};

private:

	void createShaderProgram(const std::string& shader_name);
	void setVertexInputDescriptions(const VertexDescription& vertex_desc);
	void createPipelineStates();
	void createPipelineLayout();


	//Constructor parameters (MORE TO BE ADDED)
	VkPrimitiveTopology m_topology;
	VkPolygonMode		m_polygon_mode;
	VkCullModeFlags		m_cull_mode;
	VkFrontFace			m_front_face;
	DepthTest			m_depth_test;

	//Pipeline attributes
	
	;
	std::shared_ptr<ShaderProgram>			m_shader_program;

	VkRenderPass m_renderpass;

	VkPipelineVertexInputStateCreateInfo			m_vertex_input_info = {};
	VkPipelineInputAssemblyStateCreateInfo			m_input_assembly_info = {};
	VkPipelineRasterizationStateCreateInfo			m_rasterizer_info = {};
	VkPipelineMultisampleStateCreateInfo			m_multisample_state_info = {};
	VkPipelineColorBlendAttachmentState				m_color_blend_attachment_state = {};//This could be a vector if using multiple attachments in a multi-render targer rendering framework
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






class GraphicsPipeline {
public:

	//Create graphics pipeline for default renderpass
	GraphicsPipeline(const std::string& shader_name,  const VertexDescription& vertex_desc, DepthTest depth_test = DepthTest::ReadWrite, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT,/*VK_CULL_MODE_BACK_BIT*//*VK_CULL_MODE_NONE*/
		VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE);
	
	//Create graphics pipeline for specific renderpass
	GraphicsPipeline(const RenderPass& renderpass, std::string shader_name, const VertexDescription& vertex_desc, DepthTest depth_test = DepthTest::ReadWrite, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT,/*VK_CULL_MODE_BACK_BIT*//*VK_CULL_MODE_NONE*/
		VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE);

	~GraphicsPipeline(){};


	VkPipeline pipeline() const { return m_pipeline; };
	VkPipelineLayout pipelineLayout() const { return m_pipeline_layout; };
	ShaderProgram* shaderProgram() const { return m_shader_program.get(); };
	
	VkRenderPass getPipelineRenderpass() const  { return m_pipeline_builder.vk_renderpass();};

	//Change pipeline renderpass by rebuilding it
	void rebuildPipeline (const RenderPass& renderpass);

private:

	VkPipeline				m_pipeline;
	VkPipelineLayout		m_pipeline_layout;
	std::shared_ptr<ShaderProgram>	m_shader_program;

	GraphicsPipelineBuilder m_pipeline_builder;
};



}