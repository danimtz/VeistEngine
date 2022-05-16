#pragma once



#include "Veist/Graphics/Vulkan/RenderBackend.h"
//#include "Logger.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"
#include "Veist/Resources/EngineResources.h"

namespace Veist
{
class EngineResources;

class RenderModule
{
public:
	
	enum RendererType {Forward};

	static void init(GLFWwindow* window);
	static void shutdown();
	static RenderBackend* getBackend(){ return s_render_backend.get();};
	static EngineResources* resources() { return s_resources.get(); };
	
	
	
private:
	
	
	static std::unique_ptr<RenderBackend> s_render_backend;
	static std::unique_ptr<EngineResources> s_resources;
	
};

}