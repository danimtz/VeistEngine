#include "pch.h"

#include "Application.h"

namespace Veist
{

  


    Application::Application(std::string name)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        m_window = glfwCreateWindow(1280, 720, name.c_str(), nullptr, nullptr);
        glfwSetWindowSizeLimits(m_window, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);


        //Initialize main engine modules
        RenderModule::init(m_window);
        GUIModule::init(RenderModule::getBackend().get());
        InputModule::init(m_window);


        m_thread_pool = new ThreadPool();

    }


   

    Application::~Application()
    {

        delete m_thread_pool;

        GUIModule::shutdown();

        RenderModule::shutdown();

        glfwDestroyWindow(m_window);
        glfwTerminate();


    }

}