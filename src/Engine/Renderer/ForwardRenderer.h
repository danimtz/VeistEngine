#pragma once
#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"


#define GLM_DEPTH_ZERO_TO_ONE


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

//Not going to abstract vulkan for now from this class
//C++ rule of 3 maybe. thought shouldnt be needed since Renderer/ForwardRenderer etc class is only used with unique ptr in RenderModule
class ForwardRenderer : public Renderer
{
public:

	ForwardRenderer() = default;
	~ForwardRenderer();

	virtual void init(std::shared_ptr<RenderBackend> backend) override;
	virtual void renderScene(Scene *scene) override;

private:




	//Uniform buffers here im thinking. or in a struct, but in this calss for now

};

