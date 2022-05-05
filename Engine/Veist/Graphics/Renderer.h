#pragma once

//#include <memory>

#include "Veist/Scenes/ECS/EntityRegistry.h"

namespace Veist
{

class RenderBackend;
class CommandBuffer;
class Scene;

class Renderer //Effectively an abstract class, but not abstract due to need of preinitialization
{
public:
	Renderer() = default;
	virtual ~Renderer(){};

	virtual void init(std::shared_ptr<RenderBackend> backend){};
	
	virtual void onUpdate() {};

	virtual void setScene(Scene* scene){};//TODO: Deprecate this function later on. for now just calls setECSRegistry 

	virtual void setECSRegistry(ecs::EntityRegistry* ecsRegistry) {};

	virtual void renderSceneECS(CommandBuffer& cmd_buffer) {}; //TODO: rework this. i dont like calling this from here

protected:

	virtual void setRenderBackend(std::shared_ptr<RenderBackend> backend) { m_render_backend = backend; };
	std::shared_ptr<RenderBackend> m_render_backend;

};


}