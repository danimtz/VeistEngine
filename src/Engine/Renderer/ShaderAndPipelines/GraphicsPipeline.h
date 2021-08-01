#pragma once

#include<memory>
#include<string>
#include "Engine/Renderer/Buffers/Buffer.h"

#include <glm/glm.hpp>


//This definately needs to go somewhere else in the future.(shader class? material class?)
struct MatrixPushConstant//Very specific and temporary name, so that i know to change it later on
{
	glm::mat4 MVPmatrix;
	glm::mat4 Nmatrix;
};


enum class DepthTest {
	None = 0,
	Read = 1,
	Write = 2,
	ReadWrite = Read | Write
};




//API abstraction class
class GraphicsPipeline
{
public:
	
	GraphicsPipeline() = default;
	virtual ~GraphicsPipeline(){};

	virtual void* getPipeline() const = 0;
	virtual void* getPipelineLayout() const = 0;

	static std::shared_ptr<GraphicsPipeline> Create(std::string shader_name, const VertexDescription& vertex_description);
};

