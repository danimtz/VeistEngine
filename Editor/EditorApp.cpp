#include <pch.h>

#include "EditorApp.h"

#include <Veist.h>
#include <Veist/Core/EntryPoint.h> //TODO move engine into Veist folder


namespace Veist
{

	void EditorApp::runClient()
	{
        double previousTime = glfwGetTime();
        int frameCount = 0;
        float m_last_frame_time = 0.0f;

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

            RenderModule::onUpdate(); //TODO change this from onUpdate to more customaziable. have beginFrame Present at the end etc etc. must have choosable framebuffer




            //Renderer swapchain present()





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

        RenderModule::setECSRegistry(scene->ecsRegistry());

    }

    void EditorApp::shutdownClient()
    {
        
        RenderModule::getRenderBackend()->waitIdle(); //This is needed to make sure images are not in use. probably wont be necessary once asset system is implemented 
        delete scene; //Delete scene including vkDeleteImage in destructor of textures (will be done in asset/resource system later on since they are shared pointers)

    }
    
	

	Application* CreateApplication()
	{
		return new EditorApp();
	}

	
}