#pragma once
#include "Renderer/Renderer.h"
#include "Renderer/GraphicsPipeline.h"

#include "Renderer/Mesh.h"

//Not going to abstract vulkan for now from this class
//C++ rule of 3 maybe. thought shouldnt be needed since Renderer/ForwardRenderer etc class is only used with unique ptr in RenderModule
class ForwardRenderer : public Renderer
{
public:

	ForwardRenderer() = default;
	~ForwardRenderer();

	virtual void init(std::shared_ptr<RenderBackend> backend) override;
	virtual void renderScene() override;

private:

	std::shared_ptr<GraphicsPipeline> m_pipeline; 


	//SHOULDNT BE HERE BUT IS FOR NOW UNTIL SCENE CLASS IS MADE:
	Mesh test_mesh;
};

