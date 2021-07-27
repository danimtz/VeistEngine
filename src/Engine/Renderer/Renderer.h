#pragma once


#include "Engine/Renderer/RenderBackend.h"
#include "Engine/Scenes/Scene.h"
#include <memory>

class Renderer //Effectively an abstract class, but not abstract due to need of preinitialization
{
public:
	Renderer() = default;
	virtual ~Renderer(){};

	virtual void init(std::shared_ptr<RenderBackend> backend){};
	
	virtual void renderScene(Scene* scene){};
	
protected:

	virtual void setRenderBackend(std::shared_ptr<RenderBackend> backend) { m_render_backend = backend; };
	std::shared_ptr<RenderBackend> m_render_backend;

};

