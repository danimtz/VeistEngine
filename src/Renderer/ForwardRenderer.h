#pragma once
#include "Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanGraphicsPipeline.h"

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

	VulkanGraphicsPipeline* m_pipeline; 

};

