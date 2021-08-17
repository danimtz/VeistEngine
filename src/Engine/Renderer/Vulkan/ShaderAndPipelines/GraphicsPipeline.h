#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include "Engine/Logger.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/Shader.h"
#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"

#include <glm/glm.hpp>

//This definately needs to go somewhere else in the future.(shader class? material class?)
struct MatrixPushConstant//Very specific and temporary name, so that i know to change it later on
{
	glm::mat4 model_mat;
	glm::mat4 normal_mat;
};


enum class DepthTest {
	None = 0,
	Read = 1,
	Write = 2,
	ReadWrite = Read | Write
};


class GraphicsPipeline {
public:

	GraphicsPipeline(std::string shader_name,  const VertexDescription& vertex_desc, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_NONE/*VK_CULL_MODE_BACK_BIT*/,
		VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE, DepthTest depth_test = DepthTest::ReadWrite);

	~GraphicsPipeline(){};


	VkPipeline pipeline() const { return m_pipeline; };
	VkPipelineLayout pipelineLayout() const { return m_pipeline_layout; };
	ShaderProgram* shaderProgram() const { return m_shader_program.get(); };

private:

	VkPipeline				m_pipeline;
	VkPipelineLayout		m_pipeline_layout;
	std::shared_ptr<ShaderProgram>	m_shader_program;
};


class GraphicsPipelineBuilder {
public:

	GraphicsPipelineBuilder(std::string shader_name, const VertexDescription&vertex_desc,  VkPrimitiveTopology topology,
		VkPolygonMode polygon_mode, VkCullModeFlags cull_mode,
		VkFrontFace front_face, DepthTest depth_test);
	

private:
	

	void createShaderProgram(std::string shader_name);
	void setVertexInputDescriptions(const VertexDescription& vertex_desc);
	void createPipelineStates();
	void createPipelineLayout();
	void createPipeline();


	//Constructor parameters (MORE TO BE ADDED)
	VkPrimitiveTopology m_topology;
	VkPolygonMode		m_polygon_mode;
	VkCullModeFlags		m_cull_mode;
	VkFrontFace			m_front_face;
	DepthTest			m_depth_test;

	//Pipeline attributes
	VkPipeline										m_pipeline;


	std::shared_ptr<ShaderProgram>			m_shader_program;

	
	VkPipelineVertexInputStateCreateInfo			m_vertex_input_info = {};
	VkPipelineInputAssemblyStateCreateInfo			m_input_assembly_info = {};
	VkPipelineRasterizationStateCreateInfo			m_rasterizer_info = {};
	VkPipelineMultisampleStateCreateInfo			m_multisample_state_info = {};
	VkPipelineColorBlendAttachmentState				m_color_blend_attachment_state = {};//This could be a vector if using multiple attachments in a multi-render targer rendering framework
	VkPipelineColorBlendStateCreateInfo				m_color_blend_state_info = {}; 
	VkPipelineDepthStencilStateCreateInfo			m_depth_stencil_state_info = {};
	VkViewport										m_viewport = {};
	VkRect2D										m_scissor = {};
	VkPipelineViewportStateCreateInfo				m_viewport_state_info = {};

	std::vector<VkVertexInputBindingDescription>	m_vertex_bindings;
	std::vector<VkVertexInputAttributeDescription>	m_vertex_attributes;

	VkPipelineLayout								m_pipeline_layout;


	friend class GraphicsPipeline;

};


