#pragma once



#include "Veist/Graphics/Vulkan/RenderBackend.h"
#include "Veist/Graphics/Renderer.h"
//#include "Logger.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"


namespace Veist
{

class RenderModule
{
public:
	
	enum RendererType {Forward};

	static void init(GLFWwindow* window);
	static void shutdown();
	static std::shared_ptr<RenderBackend> getBackend(){return s_render_backend;};
	static void selectRenderArchitecture(RendererType renderer_type = RendererType::Forward); //default to forward renderer

	static inline void onUpdate()
	{
		s_renderer->onUpdate();
	}
	
	static inline void renderScene(CommandBuffer& cmd)
	{
		s_renderer->renderSceneECS(cmd);
	}

	static inline void setECSRegistry(ecs::EntityRegistry* ecsRegistry)
	{
		s_renderer->setECSRegistry(ecsRegistry);
	}
	
private:

	static std::unique_ptr<Renderer> s_renderer; //THESES NED TO SOMEHOW BE PREINITIALIZED. FIGURE IT OUT TOMMORROW
	static std::shared_ptr<RenderBackend> s_render_backend;
	
};

}