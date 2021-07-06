#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "Logger.h"


struct GPUinfo_t {
    VkPhysicalDevice                        device;
    VkPhysicalDeviceProperties              properties;
    VkPhysicalDeviceMemoryProperties        memory_properties;
    VkPhysicalDeviceFeatures                features;
    VkSurfaceCapabilitiesKHR                surface_capabilities;
    std::vector<VkSurfaceFormatKHR>         surface_formats;
    std::vector<VkPresentModeKHR>           present_modes;
    std::vector<VkQueueFamilyProperties>    queue_family_properties;
    std::vector<VkExtensionProperties>      extension_properties;
};



class RenderBackend_VK { /*: RenderBackend*/// VK_RenderBackend should extend a general backend class to allow other apis
public:
    RenderBackend_VK() = delete;
    RenderBackend_VK(GLFWwindow* window);
    void init();
    void cleanup();

private://main vulkan setup
    void initContext_VK();

    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void choosePhysicalDevice();
    void createDeviceAndQueues();
    void createSwapchain();


private:
    GLFWwindow*                 m_glfw_window; //could be abstracted to use different library other than glfw, hard coded for now
    VkInstance                  m_instance;
    VkDebugUtilsMessengerEXT    m_debug_messenger;
    GPUinfo_t                   m_gpu_info;
    VkDevice                    m_device;
    uint32_t                    m_present_family_idx;
    uint32_t                    m_graphics_family_idx;
    VkQueue                     m_graphics_queue;
    VkQueue                     m_present_queue;
    VkSurfaceKHR                m_surface;
    VkSwapchainKHR                 m_swapchain;
    std::vector<const char*>    m_device_extensions;
    std::vector<const char*>    m_validation_layers;


};
