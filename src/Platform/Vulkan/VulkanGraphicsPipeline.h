#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include "Logger.h"
#include "Renderer/RenderModule.h"
#include "Renderer/GraphicsPipeline.h"

class VulkanGraphicsPipeline : public GraphicsPipeline {
public:
	VulkanGraphicsPipeline(std::string shader_name, std::string folder_path, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL, VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT,
		VkFrontFace front_face = VK_FRONT_FACE_CLOCKWISE);
	~VulkanGraphicsPipeline(){};

	virtual void* getPipeline() override { return m_pipeline; };

private:
	
	void createShaderProgram(const char* file_path, VkShaderStageFlagBits stage);
	void createPipelineStates();
	void createPipelineLayout();
	void createPipeline();

	//Constructor parameters
	VkPrimitiveTopology m_topology;
	VkPolygonMode		m_polygon_mode;
	VkCullModeFlags		m_cull_mode;
	VkFrontFace			m_front_face;

	//Pipeline attributes
	VkPipeline										m_pipeline;

	std::vector<VkShaderModule>						m_shader_module;//This might be a vector of shader modules later on since one is needed ofr each vertex shader and each fragment shader etc etc
	std::vector<VkPipelineShaderStageCreateInfo>	m_stages;
	VkPipelineVertexInputStateCreateInfo			m_vertex_input_info = {};
	VkPipelineInputAssemblyStateCreateInfo			m_input_assembly_info = {};
	VkPipelineRasterizationStateCreateInfo			m_rasterizer_info = {};
	VkPipelineMultisampleStateCreateInfo			m_multisample_state_info = {};
	VkPipelineColorBlendAttachmentState				m_color_blend_attachment_state = {};//This ocould be a vector if using multiple attachments in a multi-render targer rendering framework
	VkPipelineColorBlendStateCreateInfo				m_color_blend_state_info = {}; 
	VkViewport										m_viewport = {};
	VkRect2D										m_scissor = {};
	VkPipelineViewportStateCreateInfo				m_viewport_state_info = {};

	VkPipelineLayout								m_pipeline_layout;



};