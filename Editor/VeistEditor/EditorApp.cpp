#include <pch.h>

#include "EditorApp.h"

#include <Veist/Core/EntryPoint.h> 

//Add include "Panels.h" which shjould be a list of includes of all panels like components in ecs
#include "Panels/EngineViewportPanel.h"
#include "Panels/StatsPanel.h"
namespace VeistEditor
{

    EditorApp* EditorApp::s_Instance = nullptr;

    EditorApp::EditorApp() : Application("Editor")
    {
        s_Instance = this;

        m_ui_panels = std::make_unique<PanelManager>();

        m_ui_panels->addPanel<EngineViewportPanel>();
        m_ui_panels->addPanel<StatsPanel>();



    }


    void EditorApp::loadScene()
    {
    //TODO rework this. currently calling this twice just loads another identical scene in the same registry
  
        m_thread_pool->spawnThread( [=]()
        { 
            m_scene_loaded = false; //TODO add lock here since changing variable on another thread might data race;
            Scene::loadScene(scene->ecsRegistry()); 
            m_editor_camera = std::make_shared<CameraController>(scene->getMainCamera());
            m_scene_loaded = true;
        });
    }



	void EditorApp::runClient()
	{
        double previousTime = glfwGetTime();
        int frameCount = 0;

        auto& render_backend = RenderModule::getBackend();

        

        while (m_running)
        {
            float time = (float)glfwGetTime();
            m_frametime = time - m_last_frame_time;
            m_last_frame_time = time;

           
            if(!m_minimized){
            
                InputModule::onUpdate();

                //Update Scenes and scene related items
                {
                    if(m_scene_loaded)
                    {
                        m_editor_camera->onUpdate(m_frametime);
                    }
                    scene->onUpdate(m_frametime);
                }
           

                //Rendering
                {
                    render_backend->getSwapchain()->beginNextFrame(); //Swapchain should maybe belong to window but that would require changing the whole backend
                    CommandBuffer& cmd_buffer = render_backend->getCurrentCmdBuffer();
                    cmd_buffer.begin();
                    GUIModule::beginFrame();


                    m_ui_panels->onUpdate();
                    ImGui::ShowDemoWindow();


                    GUIModule::endFrame();


                    cmd_buffer.end();
                    render_backend->getSwapchain()->present(cmd_buffer);
                }
               

            }

            m_window->onUpdate();
            
           
        }
	}



    //Process events
    void EditorApp::onEvent(Event& event)
    {
        if (m_scene_loaded)
        {
            m_editor_camera->onEvent(event);
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
