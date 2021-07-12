#pragma once


#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include "Logger.h"

#include "Renderer/RenderBackend.h"


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



class RenderBackend_Vulkan : public RenderBackend { 
public:

    virtual void init(GLFWwindow* window) override;
    virtual void shutdown() override;

    //Void pointers to work with abstraction. Needs static cast when called
    virtual void* getDevice() override { return m_device; }; 
    virtual void* getSwapchainExtent() override { return &m_swapchain_extent; };
    virtual void* getRenderPass() override { return m_render_pass; };

//render commands/functions that use vulkan commands
    virtual void RC_beginFrame() override;
    virtual void RC_endFrame() override;
    virtual void RC_bindGraphicsPipeline(GraphicsPipeline *pipeline) override;
    virtual void RC_drawSumbit() override;

private://main vulkan setup
    void initContext_VK();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void choosePhysicalDevice();
    void createDeviceAndQueues();
    void createSwapchain();
    void createCommandPoolAndBuffers();
    void createDefaultRenderPass();
    void createFramebuffers();
    void createSemaphoresAndFences();

private:
    GLFWwindow*                     m_glfw_window; //could be abstracted to use different library other than glfw, hard coded for now
    VkInstance                      m_instance;
    VkDebugUtilsMessengerEXT        m_debug_messenger;
    GPUinfo_t                       m_gpu_info;
    VkDevice                        m_device;
    uint32_t                        m_present_family_idx;
    uint32_t                        m_graphics_family_idx;
    VkQueue                         m_graphics_queue;
    VkQueue                         m_present_queue;
    VkSurfaceKHR                    m_surface;

 
    VkRenderPass                    m_render_pass;//default renderpass
    std::vector<VkFramebuffer>      m_framebuffers;


    VkSwapchainKHR                  m_swapchain;
    VkFormat                        m_swapchain_format;
    VkExtent2D                      m_swapchain_extent;
    VkPresentModeKHR                m_swapchain_present_mode;
    std::vector<VkImage>            m_swapchain_images;
    std::vector<VkImageView>        m_swapchain_views;
    uint32_t                        m_swapchain_img_idx;


    VkCommandPool                   m_command_pool;
    std::vector<VkCommandBuffer>    m_command_buffers;


    VkSemaphore                     m_present_semaphore, m_render_semaphore;
    VkFence                         m_render_fence;


    unsigned int                    m_frame_number{0};


    std::vector<const char*>    m_device_extensions;
    std::vector<const char*>    m_validation_layers;

    bool                        m_isInitialized{false};
};
