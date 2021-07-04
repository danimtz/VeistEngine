#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <iostream>

class VK_RenderBackend { /*: RenderBackend*/// VK_RenderBackend should extend a general backend class to allow other apis
public:
    VK_RenderBackend(GLFWwindow* window);
    void init();
    void cleanup();

    static void debugprint(){std::cout << "Heeeloo there"<<std::endl;};
private:
    void init_vulkan();


private:
    GLFWwindow* m_glfw_window;

};
