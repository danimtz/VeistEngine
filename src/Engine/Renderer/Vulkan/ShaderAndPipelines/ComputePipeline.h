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
	ComputePipeline() = default;

private:
	
	VkPipeline				m_pipeline;
	VkPipelineLayout		m_pipeline_layout;
	std::shared_ptr<ShaderProgram>	m_shader_program;

};

