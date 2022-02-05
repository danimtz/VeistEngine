#include <pch.h>

#include "EditorApp.h"

#include <Veist/Core/EntryPoint.h> 

//Add include "Panels.h" which shjould be a list of includes of all panels like components in ecs
#include "Panels/EngineViewportPanel.h"
namespace VeistEditor
{

    EditorApp* EditorApp::s_Instance = nullptr;

    EditorApp::EditorApp() : Application("Editor")
    {
        s_Instance = this;

        m_ui_panels = std::make_unique<PanelManager>();

        m_ui_panels->addPanel<EngineViewportPanel>();

    }


    void EditorApp::loadScene()
    {
    //TODO rework this. currently calling this twice just loads another identical scene in the same registry
  
        m_thread_pool->spawnThread( [=](){ Scene::loadScene(scene->ecsRegistry()); }  );
    }



	void EditorApp::runClient()
	{
        double previousTime = glfwGetTime();
        int frameCount = 0;
        float m_last_frame_time = 0.0f;


        auto& render_backend = RenderModule::getBackend();

        

        while (m_running)
        {
            float time = (float)glfwGetTime();
            Timestep timestep = time - m_last_frame_time;
            m_last_frame_time = time;

            std::cout << timestep.getMilliseconds() << "ms" << std::endl;

            //FPS counter
                /*double currentTime = glfwGetTime();
                frameCount++;
                if (currentTime - previousTime >= 1.0)
                {
                    std::cout << frameCount << std::endl;
                    frameCount = 0;
                    previousTime = currentTime;
                }*/


            InputModule::onUpdate();
            scene->onUpdate(timestep);

            
            

            
            //RenderModule::onUpdate(); //TODO change this from onUpdate to more customaziable. have beginFrame Present at the end etc etc. must have choosable framebuffer
            
            
            render_backend->getSwapchain()->beginNextFrame(); //Swapchain should maybe belong to window but that would require changing the whole backend

            CommandBuffer& cmd_buffer = render_backend->getCurrentCmdBuffer();
            cmd_buffer.begin();

            GUIModule::beginFrame();



            //Renderer swapchain present()
            m_ui_panels->onUpdate();

            //ImGui::ShowDemoWindow();


            GUIModule::endFrame();


            cmd_buffer.end();
            render_backend->getSwapchain()->present(cmd_buffer);



            if (glfwWindowShouldClose(m_window))
            {
                m_running = false;
            }
            //Rendering (rework this at some point RenderModule::onUpdate() should be called here only but then GUI module depends on render module etc etc) 

        }



	}



    void EditorApp::initClient()
    {

        
        scene = new Scene();//TEMPORARY

        

        //Scene::loadScene( scene->ecsRegistry() );
        //Scene::loadScene(EditorApp::get().getActiveScene()->ecsRegistry());

        RenderModule::setECSRegistry(scene->ecsRegistry());

        
    }

    void EditorApp::shutdownClient()
    {
        
        RenderModule::getBackend()->waitIdle(); //This is needed to make sure images are not in use. probably wont be necessary once asset system is implemented 
        delete scene; //Delete scene including vkDeleteImage in destructor of textures (will be done in asset/resource system later on since they are shared pointers)

    }
    
	

  
 }

 
namespace Veist
{



    Application* CreateApplication()
    {
        return new VeistEditor::EditorApp();
    }
}
