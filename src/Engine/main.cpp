



#include "Renderer/RenderModule.h"
#include "ImGUI/GUIModule.h"
#include "Input/InputModule.h"
#include "Engine/Scenes/Scene.h"




int main() {



//Init window (should be part of window module)

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan", nullptr, nullptr);
    glfwSetWindowSizeLimits(window, 100, 100, GLFW_DONT_CARE, GLFW_DONT_CARE);
    

//Initialize main engine modules

    RenderModule::init(window);
    GUIModule::init(RenderModule::getRenderBackend().get());
    InputModule::init(window);

    Scene* scene = new Scene();

    RenderModule::setScene(scene);

//Mainloop
    while (!glfwWindowShouldClose(window)) 
    {

        InputModule::onUpdate();

        scene->onUpdate();

        RenderModule::onUpdate();


        //Rendering (rework this at some point RenderModule::onUpdate() should be called here only but then GUI module depends on render module etc etc) 
       
    }


//Cleanup Vulkan and window
    
    GUIModule::shutdown();
    RenderModule::shutdown();

    delete scene;

    glfwDestroyWindow(window);
    glfwTerminate();
    

    return 0;
}