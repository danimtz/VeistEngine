#pragma once
#include <memory>
#include "Renderer/Renderer.h"
#include "Renderer/RenderBackend.h"
#include "Logger.h"
#include <GLFW/glfw3.h>

class RenderModule
{
public:
	
	enum RendererType {Forward, Deferred, Forwardplus};

	static void init(GLFWwindow* window);
	static void shutdown();
	static std::shared_ptr<RenderBackend> getRenderBackend(){return s_render_backend;};
	static void selectRenderArchitecture(RendererType renderer_type = RendererType::Forward); //default to forward renderer

	
	static inline void renderScene()
	{
		s_renderer->renderScene();
	};

private:

	static std::unique_ptr<Renderer> s_renderer; //THESES NED TO SOMEHOW BE PREINITIALIZED. FIGURE IT OUT TOMMORROW
	static std::shared_ptr<RenderBackend> s_render_backend;
	
};

