



#include "Renderer/RenderModule.h"
#include "ImGUI/GUIModule.h"
#include "Input/InputModule.h"
#include "Engine/Scenes/Scene.h"




int main() {



//Init window (should be part of window module)

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan", nullptr, nullptr);


//Initialize main engine modules

    RenderModule::init(window);
    GUIModule::init(RenderModule::getRenderBackend().get());
    InputModule::init(window);

    Scene* scene = new Scene();

//Mainloop
    while (!glfwWindowShouldClose(window)) 
    {

        InputModule::onUpdate();

        scene->onUpdate();


        //Rendering (rework this at some point RenderModule::onUpdate() should be called here only but then GUI module depends on render module etc etc) 
        RenderModule::beginFrame();
        {

            //Render scene
            RenderModule::renderScene(scene);
           

            //Render GUI
            GUIModule::beginFrame();
            {
                //module or whatever->onUpdateImGUI()
                ImGui::ShowDemoWindow();
            }
            GUIModule::endFrame();
        }
        RenderModule::endFrame();
        

    }


//Cleanup Vulkan and window
    
    GUIModule::shutdown();
    RenderModule::shutdown();

    delete scene;

    glfwDestroyWindow(window);
    glfwTerminate();
    

    return 0;
}