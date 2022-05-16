#include "pch.h"
#include "Veist/Graphics/RenderModule.h"


namespace Veist
{


    std::unique_ptr<EngineResources> RenderModule::s_resources = nullptr;
    std::unique_ptr<RenderBackend> RenderModule::s_render_backend = nullptr;

    void RenderModule::init(GLFWwindow* window) 
    {
	
        s_render_backend = std::make_unique<RenderBackend>();
        s_render_backend->init(window);

        
        s_resources = std::make_unique<EngineResources>();
    }


    void RenderModule::shutdown()
    {

        s_resources.reset(); 
        s_render_backend->shutdown();
    
    }


}