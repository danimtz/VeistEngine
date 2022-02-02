#pragma once


#include "Application.h"


extern Veist::Application* Veist::CreateApplication();


int main()
{


    //Veist::Application app = { "vulkan" };
    //app.run();
    auto app = Veist::CreateApplication();

    app->run();

    delete app;

    
}