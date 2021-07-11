
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>


#include <iostream>
#include <memory>

#include "Renderer/RenderBackend.h"

//Application => Engine => Renderer? , RendererBackend => VK_RenderBackend class flow



int main() {

//Init window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);

  

//Init render backend
    std::unique_ptr<RenderBackend> vulkan_backend = RenderBackend::CreateBackend(window);
    vulkan_backend->init();


//Mainloop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        //Renderer::renderscene or update or whatever
            vulkan_backend->RC_beginFrame();
            //draw commands
            vulkan_backend->RC_endFrame();
    }


//Cleanup Vulkan and window
    vulkan_backend->shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}