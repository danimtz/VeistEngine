#pragma once

#include<memory>
#include<string>
#include "Renderer/Buffer.h"

class GraphicsPipeline
{
public:
	
	GraphicsPipeline() = default;
	virtual ~GraphicsPipeline(){};

	virtual void* getPipeline() const = 0;

	static std::shared_ptr<GraphicsPipeline> Create(std::string shader_name, std::string folder_path, VertexDescription &vertex_description);
};

