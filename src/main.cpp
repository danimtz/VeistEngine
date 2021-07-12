

#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

#include "Renderer/RenderModule.h"




int main() {

//Init window

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
 
    RenderModule::init(window);
    

    
//Mainloop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        RenderModule::renderScene();
    }


//Cleanup Vulkan and window
    
    RenderModule::shutdown();

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}