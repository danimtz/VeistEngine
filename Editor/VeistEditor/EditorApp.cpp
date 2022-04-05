#include <pch.h>

#include "EditorApp.h"

#include <Veist/Core/EntryPoint.h> 

#include "Veist/Events/EditorEvents.h"

//Add include "Panels.h" which shjould be a list of includes of all panels like components in ecs
#include "Panels/Panels.h"


namespace VeistEditor
{

    EditorApp* EditorApp::s_Instance = nullptr;

    EditorApp::EditorApp() : Application("Editor")
    {
        s_Instance = this;

    }


    void EditorApp::loadScene()
    {
    //TODO rework this. currently calling this twice just loads another identical scene in the same registry
        m_scene_loaded = false;
        m_thread_pool->spawnThread( [=]()
        { 
            //TODO: DONT USE SCENE CLASS FUNCITON LATEER. USE SCENE SERIALIZER. This is simulating creating a new scene
            m_active_scene = std::make_unique<Scene>();
            m_active_scene->loadScene(m_active_scene->ecsRegistry()); 

            //Send event
            EditorSceneChangedEvent event(m_active_scene.get());
            EditorApp::get().processEvent(event);
            RenderModule::setECSRegistry(m_active_scene->ecsRegistry());//TOOD Change the need to use this function when renderer reworked into framegraph
            m_scene_loaded = true; //Might need a lock for concurrency
        });
    }



	void EditorApp::runClient()
	{
        double previousTime = glfwGetTime();
        int frameCount = 0;

        auto& render_backend = RenderModule::getBackend();

        
        loadScene();
        while (m_running)
        {
            float time = (float)glfwGetTime();
            m_frametime = time - m_last_frame_time;
            m_last_frame_time = time;

           
            if(!m_minimized && m_scene_loaded){
            
                InputModule::onUpdate();

                //Update Scene and scene related items (game simulation)
                {
                    m_active_scene->onUpdate(m_frametime);
                }


                //Rendering
                {
                    render_backend->getSwapchain()->beginNextFrame(); //Swapchain should maybe belong to window but that would require changing the whole backend
                    CommandBuffer& cmd_buffer = render_backend->getCurrentCmdBuffer();
                    
                    cmd_buffer.begin();
                    GUIModule::beginFrame();//why do i need guimodule then?

                    m_ui_panels->onUpdate();//dockspace set up here
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

        m_ui_panels->onEvent(event);
        
    }



    void EditorApp::initClient()
    {

        m_active_scene = std::make_unique<Scene>();//TEMPORARY

        
        RenderModule::setECSRegistry(m_active_scene->ecsRegistry());

        m_ui_panels = std::make_unique<PanelManager>();

        m_ui_panels->addPanel<EngineViewportPanel>();
        m_ui_panels->addPanel<StatsPanel>();
        m_ui_panels->addPanel<HierarchyPanel>();
        m_ui_panels->addPanel<EntityPropertiesPanel>();
    }



    void EditorApp::shutdownClient()
    {
        
        RenderModule::getBackend()->waitIdle(); //This is needed to make sure images are not in use. probably wont be necessary once asset system is implemented 
        //delete scene; //Delete scene including vkDeleteImage in destructor of textures (will be done in asset/resource system later on since they are shared pointers)

    }
    
	

  
 }

 
namespace Veist
{



    Application* CreateApplication()
    {
        return new VeistEditor::EditorApp();
    }
}
