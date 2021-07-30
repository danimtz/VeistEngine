



#include "Renderer/RenderModule.h"
#include "ImGUI/GUIModule.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"

int main() {

//Init window

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);


    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vulkan", nullptr, nullptr);

    RenderModule::init(window);
    //ImGui::CreateContext();
    GUIModule::init();

    Scene* scene = new Scene();

//Mainloop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();


        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //imgui commands
        ImGui::ShowDemoWindow();

      


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