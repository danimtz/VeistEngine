#include "pch.h"
#include "Veist/Graphics/RenderModule.h"
#include "Veist/Graphics/ForwardRenderer.h"


namespace Veist
{


    std::unique_ptr<Renderer> RenderModule::s_renderer = nullptr;
    std::unique_ptr<EngineResources> RenderModule::s_resources = nullptr;
    std::shared_ptr<RenderBackend> RenderModule::s_render_backend = nullptr;

    void RenderModule::init(GLFWwindow* window) 
    {
	
        s_render_backend = std::make_shared<RenderBackend>();
        s_render_backend->init(window);

        selectRenderArchitecture(); //By default create forward renderer. Later make init function with custom arguemtn to initialize this to something else

        s_resources = std::make_unique<EngineResources>();
    }


    void RenderModule::shutdown()
    {

        s_render_backend->shutdown();
    
    }


    void RenderModule::selectRenderArchitecture(RendererType renderer_type)
    {
        //Select rendering architecture: forward/deferred etc...

        //Forward only for now
        switch(renderer_type){

            case RendererType::Forward:
            
                s_renderer = std::make_unique<ForwardRenderer>();
                s_renderer->init(s_render_backend);

                CONSOLE_LOG("Forward renderer created");
                break;

            default:
                CRITICAL_ERROR_LOG("Could not create renderer: Renderer type selected not supported");
                break;
        }
    }

}