



#include "Renderer/RenderModule.h"
#include "ImGUI/GUIModule.h"

#include <imgui.h>

int main() {

//Init window

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan", nullptr, nullptr);

    RenderModule::init(window);
    ImGui::CreateContext();
    GUIModule::init();

    Scene* scene = new Scene();

//Mainloop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        scene->onUpdate();
        RenderModule::renderScene(scene);
    }

    

//Cleanup Vulkan and window
    
    GUIModule::shutdown();
    RenderModule::shutdown();

    delete scene;

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}