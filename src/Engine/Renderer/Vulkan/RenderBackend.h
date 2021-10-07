 
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
#include "Engine/Renderer/Vulkan/Framebuffers/Swapchain.h"
#include "Engine/Renderer/Vulkan/Commands/CommandPool.h"

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
    VkCommandBuffer m_command_buffer;
};


struct GPUUploadContext
{
    VkFence m_fence;
    std::shared_ptr<CommandPool> m_command_pool;
};


class RenderBackend {
public:

    RenderBackend() = default;
    void init(GLFWwindow* window);
    void shutdown();
    void initImGUI();

    
    GLFWwindow* getWindow() const { return m_glfw_window;};
    VkDevice getDevice() const { return m_device; }; 
    const VkExtent2D& getSwapchainExtent() const { return m_swapchain.get()->extent(); };
    VkRenderPass getRenderPass() const { return m_render_pass.renderpass(); };
    VkSurfaceKHR getSurface() const { return m_surface; };
    VmaAllocator getAllocator() const { return m_allocator; }; //CONSIDER MOVING ALLOCATOR TO SEPARATE CLASS
    const GPUinfo_t& getGPUinfo() const { return m_gpu_info; };

    uint32_t getGraphicsFamily() const {return m_graphics_family_idx;};

    DescriptorSetAllocator* getDescriptorAllocator() const { return m_descriptor_allocator.get(); };
    
    const uint32_t getSwapchainBufferCount() const {return FRAME_OVERLAP_COUNT; };
    uint32_t getFrameNumber() const { return m_frame_count; };
    uint32_t getSwapchainImageNumber() const { return (m_frame_count % FRAME_OVERLAP_COUNT); };
    void pushToDeletionQueue(std::function<void()> function);
    void pushToSwapchainDeletionQueue(std::function<void()> function);

    void immediateSubmit(std::function<void(VkCommandBuffer cmd)> function);
    VkCommandBuffer getCurrentCmdBuffer() { return getCurrentFrame().m_command_buffer; };

    std::shared_ptr<CommandPool> getCommandPool(uint32_t thread_id = 0);
    
public:
    //render commands/functions that use vulkan commands
    void RC_beginFrame();
    void RC_endFrame();


    //NOTE TO FUTURE ME should renderbackend have these funcitons? or should GraphicsPipeline and VertexBUffer etc have a  Bind() function that does them
    void RC_bindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>); 
    void RC_pushConstants(const std::shared_ptr<GraphicsPipeline> pipeline, const MatrixPushConstant push_constant);
    void RC_bindVertexBuffer(const std::shared_ptr<VertexBuffer> vertex_buffer);
    void RC_bindIndexBuffer(const std::shared_ptr<IndexBuffer> index_buffer);

    void RC_bindDescriptorSet(const std::shared_ptr<GraphicsPipeline> pipeline, const DescriptorSet& descriptor_set, uint32_t offset_count = 0, uint32_t* p_dynamic_offset = nullptr);
    void RC_drawIndexed(uint32_t size);
    void RC_drawSumbit(uint32_t size);



private://main vulkan setup

    void initContext_VK();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void choosePhysicalDevice();
    void createDeviceAndQueues();
    void createVmaAllocator();
    void createSwapchainAndImages();
    void createCommandPoolAndBuffers();
    void createDefaultRenderPass();
    void createFramebuffers();
    void createUploadSemaphoresAndFences();
    void createDescriptorAllocator();
    
    VulkanFrameData& getCurrentFrame() { return m_frame_data[m_frame_count % FRAME_OVERLAP_COUNT]; };


 
 
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
    std::map<uint32_t, std::shared_ptr<CommandPool>> m_command_pools;

    VulkanFrameData                 m_frame_data[FRAME_OVERLAP_COUNT];

//Immediate sumbit 
    GPUUploadContext                m_upload_context;


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
