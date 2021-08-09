#pragma once

#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"


//TODO
class Material
{
public:

	Material(const char* material_name, const char * material_filepath, const VertexDescription& vertex_desc);



	std::shared_ptr<GraphicsPipeline> pipeline() const { return m_pipeline; };

private:

	std::string m_name;
	std::shared_ptr<GraphicsPipeline> m_pipeline;

};

