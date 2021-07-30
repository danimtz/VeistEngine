#pragma once



#include "Engine/Renderer/Renderer.h"
#include "Engine/Renderer/RenderBackend.h"
#include "Engine/Logger.h"



class RenderModule
{
public:
	
	enum RendererType {Forward, Deferred, Forwardplus};

	static void init(GLFWwindow* window);
	static void shutdown();
	static std::shared_ptr<RenderBackend> getRenderBackend(){return s_render_backend;};
	static void selectRenderArchitecture(RendererType renderer_type = RendererType::Forward); //default to forward renderer


	static inline void beginFrame()
	{
		s_render_backend->RC_beginFrame();
	};

	static inline void endFrame()
	{
		s_render_backend->RC_endFrame();
	};
	
	static inline void renderScene(Scene* scene)
	{
		s_renderer->renderScene(scene);
	};

private:

	static std::unique_ptr<Renderer> s_renderer; //THESES NED TO SOMEHOW BE PREINITIALIZED. FIGURE IT OUT TOMMORROW
	static std::shared_ptr<RenderBackend> s_render_backend;
	
};

