#pragma once

#include<memory>
#include<string>
#include "Renderer/Buffer.h"

#include <glm/glm.hpp>


//This definately needs to go somewhere else in the future.(shader class? material class?)
struct MatrixPushConstant//Very specific and temporary name, so that i know to change it later on
{
	glm::mat4 MVPmatrix;
};


//API abstraction class
class GraphicsPipeline
{
public:
	
	GraphicsPipeline() = default;
	virtual ~GraphicsPipeline(){};

	virtual void* getPipeline() const = 0;
	virtual void* getPipelineLayout() const = 0;

	static std::shared_ptr<GraphicsPipeline> Create(std::string shader_name, std::string folder_path, const VertexDescription &vertex_description);
};

