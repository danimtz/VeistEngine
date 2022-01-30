



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
    double previousTime = glfwGetTime();
    int frameCount = 0;

    float m_last_frame_time = 0.0f; 
    while (!glfwWindowShouldClose(window)) 
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

        RenderModule::onUpdate();


        //Rendering (rework this at some point RenderModule::onUpdate() should be called here only but then GUI module depends on render module etc etc) 
       
    }


//Cleanup Vulkan and window
    
    RenderModule::getRenderBackend()->waitIdle(); //This is needed to make sure images are not in use. probably wont be necessary once asset system is implemented 

    delete scene; //Delete scene including vkDeleteImage in destructor of textures (will be done in asset/resource system later on since they are shared pointers)
    
    GUIModule::shutdown();
   
    RenderModule::shutdown();

    

    glfwDestroyWindow(window);
    glfwTerminate();
    

    return 0;
}