 
#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>
#include <map>
#include <deque>
#include <functional>
#include <set>
#include <cmath>
#include <algorithm>

#include "vk_mem_alloc.h"


#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


#include "Engine/Logger.h"
#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Buffers/IndexBuffer.h" //HAVING THESE TWO BUFFER HEADERS HERE THAT LEAD TO vk mem alloc gives redefinition errors
#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSetAllocator.h"
#include "Engine/Renderer/Vulkan/Framebuffers/RenderPass.h"
#include "Engine/Renderer/Vulkan/Framebuffers/Framebuffer.h"
#include "Engine/Renderer/Vulkan/Swapchain/Swapchain.h"
#include "Engine/Renderer/Vulkan/Commands/CommandPool.h"
#include "Engine/Renderer/Vulkan/Commands/CommandBuffer.h"

constexpr int FRAME_OVERLAP_COUNT = 3;


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

struct DeletionQueue
{
    std::deque< std::function<void()> > deletors;

    void pushFunction(std::function<void()> function) {
        deletors.push_back(function);
    }

    void executeDeletionQueue() {
        for (auto it = deletors.rbegin(); it != deletors.rend(); it++) {
            (*it)(); //call function
        }
        deletors.clear();
    }
};


struct VulkanFrameData
{
    //VkCommandPool  m_command_pool;
    CommandBuffer m_command_buffer;
};



class RenderBackend {
public:

    RenderBackend() = default;
    void init(GLFWwindow* window);
    void shutdown();
    void initImGUI();

    //TODO: clean up
    GLFWwindow* getWindow() const { return m_glfw_window;};
    VkDevice getDevice() const { return m_device; }; 
    VkSurfaceKHR getSurface() const { return m_surface; };
    const GPUinfo_t& getGPUinfo() const { return m_gpu_info; };
    VkQueue getGraphicsQueue() const { return m_graphics_queue; }
    DescriptorSetAllocator* getDescriptorAllocator() const { return m_descriptor_allocator.get(); };
    uint32_t getSwapchainImageNumber() const { return (m_frame_count % FRAME_OVERLAP_COUNT); };
    void pushToDeletionQueue(std::function<void()> function);
    void pushToSwapchainDeletionQueue(std::function<void()> function);
    const Framebuffer& getCurrentFramebuffer() const {return m_framebuffers[m_swapchain->currentImageIndex()]; };
    CommandBuffer& getCurrentCmdBuffer() { return getCurrentFrameCmdBuffer(); }; //TODO rework this later
    Swapchain* getSwapchain() const {return m_swapchain.get();};
    VmaAllocator getAllocator() const { return m_allocator; }; //CONSIDER MOVING ALLOCATOR TO SEPARATE CLASS
    CommandBuffer createDisposableCmdBuffer();
    uint32_t getGraphicsFamily() const { return m_graphics_family_idx; };
    const RenderPass& getRenderPass() const { return m_render_pass; };
    uint32_t getFrameNumber() const { return m_frame_count; };
    void incrementFrameCounter() { m_frame_count++; };

    void rebuildSwapchain();

private://main vulkan setup

    void initContext_VK();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void choosePhysicalDevice();
    void createDeviceAndQueues();
    void createVmaAllocator();

    void createSwapchainAndImages();
    void createDefaultRenderPass();
    void createFramebuffers();

    void createCommandPoolAndBuffers();
    //void createUploadSemaphoresAndFences();
    void createDescriptorAllocator();
    
    CommandBuffer& getCurrentFrameCmdBuffer() { return m_command_buffers[m_frame_count % FRAME_OVERLAP_COUNT]; };


 
 
private:
// context
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

    VmaAllocator                    m_allocator;

// Swapchain/renderpass/framebuffer
    std::unique_ptr<Swapchain>      m_swapchain;
    RenderPass                      m_render_pass;
    std::vector<Framebuffer>        m_framebuffers;
    SwapchainDepthAttachment        m_swapchain_depth_image;
  


// Per frame data(command buffers (sync structures inside swapchain)) TODO: move cmd buffers out of here
//Command buffers
    
    //std::map<uint32_t, std::shared_ptr<CommandPool>> m_command_pools;
    std::vector<std::shared_ptr<CommandPool>>   m_command_pools;
    CommandBuffer                               m_command_buffers[FRAME_OVERLAP_COUNT];
    //VulkanFrameData                 m_frame_data[FRAME_OVERLAP_COUNT];

//Immediate sumbit 
    std::shared_ptr<CommandPool>    m_disposable_pool;         


// Other
    uint32_t                    m_frame_count{0};

    std::vector<const char*>    m_device_extensions;
    std::vector<const char*>    m_validation_layers;

    bool                        m_isInitialized{false};

    DeletionQueue               m_deletion_queue;

    DeletionQueue               m_swapchain_deletion_queue;

//Descriptors

    std::unique_ptr<DescriptorSetAllocator> m_descriptor_allocator;
    
};
