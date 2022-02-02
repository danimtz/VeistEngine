#pragma once

#include <vulkan/vulkan.h>
//#include <fstream>
//#include <vector>
//#include <string>

#include "Veist/Renderer/Vulkan/ShaderAndPipelines/Shader.h"
//#include "Renderer/Vulkan/Buffers/VertexDescription.h"
//#include "Renderer/Vulkan/Framebuffers/Renderpass.h"


namespace Veist
{

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

}