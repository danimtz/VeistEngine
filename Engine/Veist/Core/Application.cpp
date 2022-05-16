#include "pch.h"

#include "Application.h"

#include "Veist/Events/Event.h"

namespace Veist
{

  

    Application::Application(std::string name)
    {

        
        


        //Initialize main engine modules
        m_window = new Window(1600, 900, name);
        m_window->setEventCallback(VEIST_EVENT_BIND_FUNCTION(Application::processEvent));
        RenderModule::init(m_window->windowHandle());
        GUIModule::init(RenderModule::getBackend());
        InputModule::init(m_window->windowHandle());


        m_thread_pool = new ThreadPool();

    }


   

    Application::~Application()
    {

        delete m_thread_pool;

        GUIModule::shutdown();

        RenderModule::shutdown();

        


    }




    void Application::processEvent(Event& event)
    {   

        EventHandler handler(event);
        handler.handleEvent<WindowCloseEvent>(VEIST_EVENT_BIND_FUNCTION(Application::onWindowClose));
        handler.handleEvent<WindowResizeEvent>(VEIST_EVENT_BIND_FUNCTION(Application::onWindowResize));

        onEvent(event);

    }




    void Application::onWindowClose(WindowCloseEvent& event)
    {
        m_running = false;
    }


    void Application::onWindowResize(WindowResizeEvent& event)
    {

        if (event.getHeight() == 0 || event.getWidth() == 0)
        {
            m_minimized = true;
            return;
        }
        m_minimized = false;

    }

}