#pragma once

#include <vulkan/vulkan.h>
//#include <fstream>
//#include <vector>
//#include <string>
#include "Engine/Logger.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/Shader.h"
//#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
//#include "Engine/Renderer/Vulkan/Framebuffers/Renderpass.h"


class ComputePipeline
{

public:

	ComputePipeline(const std::string& shader_name);


	VkPipeline pipeline() const { return m_pipeline; };
	VkPipelineLayout pipelineLayout() const { return m_pipeline_layout; };
	ShaderProgram* shaderProgram() const { return m_shader_program.get(); };

private:

	void createShaderProgram(const std::string& shader_name);
	void createPipelineLayout();
	void createPipeline();

	
	VkPipeline				m_pipeline;
	VkPipelineLayout		m_pipeline_layout;
	std::shared_ptr<ShaderProgram>	m_shader_program;

};

