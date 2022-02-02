#include "pch.h"

#include "Application.h"

namespace Veist
{

    Application* Application::s_Instance = nullptr;

    Application::Application(std::string name)
    {


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(1280, 720, name.c_str(), nullptr, nullptr);
        glfwSetWindowSizeLimits(m_window, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);


        //Initialize main engine modules
        RenderModule::init(m_window);
        GUIModule::init(RenderModule::getRenderBackend().get());
        InputModule::init(m_window);

        scene = new Scene();//TEMPORARY

        RenderModule::setECSRegistry(scene->ecsRegistry());
    }




    void Application::run()
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


        shutdown();

    }




    void Application::shutdown()
    {

        RenderModule::getRenderBackend()->waitIdle(); //This is needed to make sure images are not in use. probably wont be necessary once asset system is implemented 

        delete scene; //Delete scene including vkDeleteImage in destructor of textures (will be done in asset/resource system later on since they are shared pointers)

        GUIModule::shutdown();

        RenderModule::shutdown();



        glfwDestroyWindow(m_window);
        glfwTerminate();

    }

}