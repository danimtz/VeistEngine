#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include "Engine/Logger.h"
#include "Engine/Renderer/RenderModule.h"
#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"
#include "Platform/Vulkan/ShaderAndPipelines/VulkanShader.h"

class VulkanGraphicsPipeline : public GraphicsPipeline {
public:

	VulkanGraphicsPipeline(std::string shader_name,  const VertexDescription& vertex_desc, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_NONE/*VK_CULL_MODE_BACK_BIT*/,
		VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE, DepthTest depth_test = DepthTest::ReadWrite);

	~VulkanGraphicsPipeline(){};


	virtual void* getPipeline() const override { return m_pipeline; };
	virtual void* getPipelineLayout() const override { return m_pipeline_layout; };

private:

	VkPipeline				m_pipeline;
	VkPipelineLayout		m_pipeline_layout;

};


class VulkanGraphicsPipelineBuilder {
public:

	VulkanGraphicsPipelineBuilder(std::string shader_name, const VertexDescription&vertex_desc,  VkPrimitiveTopology topology,
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


	std::shared_ptr<VulkanShaderProgram>			m_shader_program;

	
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


	friend class VulkanGraphicsPipeline;

};


