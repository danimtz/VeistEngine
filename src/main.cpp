
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


#include <iostream>


#include "RenderBackend_VK.h"

//No Application => Engine => Renderer => VK_RenderBackend class flow. Just building vk_renderbackend for now



int main() {

//Init window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

  

//Init vulkan
    RenderBackend_VK vulkan_backend = {window};
    vulkan_backend.init();

//Mainloop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }


//Cleanup Vulkan and window
   

    vulkan_backend.cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();





    return 0;
}