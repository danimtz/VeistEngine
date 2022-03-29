#pragma once


#include "Application.h"


extern Veist::Application* Veist::CreateApplication();




int main()
{





    auto app = Veist::CreateApplication();

    app->initClient();

    app->runClient();

    app->shutdownClient();

    delete app;

    
}