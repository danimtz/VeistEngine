#pragma once

#include <memory>

class RenderBackend;
class Scene;

class Renderer //Effectively an abstract class, but not abstract due to need of preinitialization
{
public:
	Renderer() = default;
	virtual ~Renderer(){};

	virtual void init(std::shared_ptr<RenderBackend> backend){};
	
	virtual void onUpdate() {};

	virtual void setScene(Scene* scene){};

protected:

	virtual void setRenderBackend(std::shared_ptr<RenderBackend> backend) { m_render_backend = backend; };
	std::shared_ptr<RenderBackend> m_render_backend;

};

