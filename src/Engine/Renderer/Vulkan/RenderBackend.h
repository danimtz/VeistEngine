 
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




constexpr int FRAME_OVERLAP_COUNT = 2;


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

//Move this to a separate class eventually  -> image or vulkanimage. 
//Its only used for depth stencil image
struct VmaImage {
    VkImage m_image;
    VmaAllocation m_allocation;
};


struct VulkanFrameData
{
    VkCommandPool           m_command_pool;
    VkCommandBuffer         m_command_buffer;
    VkSemaphore             m_present_semaphore, m_render_semaphore;
    VkFence                 m_render_fence;
};


struct GPUUploadContext
{
    VkFence m_fence;
    VkCommandPool m_command_pool;
};


class RenderBackend {
public:

    RenderBackend() = default;
    void init(GLFWwindow* window);
    void shutdown();
    void initImGUI();

    
    GLFWwindow* getWindow() const { return m_glfw_window;};
    VkDevice getDevice() const { return m_device; }; 
    VkExtent2D* getSwapchainExtent() { return &m_swapchain_extent; };
    VkRenderPass getRenderPass() const { return m_render_pass; };
    VmaAllocator getAllocator() const { return m_allocator; }; //CONSIDER MOVING ALLOCATOR TO SEPARATE CLASS
    const GPUinfo_t& getGPUinfo() const { return m_gpu_info; }

    DescriptorSetAllocator* getDescriptorAllocator() const { return m_descriptor_allocator.get(); };
    
    const uint32_t getSwapchainBufferCount() const {return FRAME_OVERLAP_COUNT; };
    uint32_t getFrameNumber() const { return m_frame_count; };
    uint32_t getSwapchainImageNumber() const { return (m_frame_count % FRAME_OVERLAP_COUNT); };
    void pushToDeletionQueue(std::function<void()> function);

    
public:
    //render commands/functions that use vulkan commands
    void RC_beginFrame();
    void RC_endFrame();

    //NOTE TO FUTURE ME should renderbackend have these funcitons? or should GraphicsPipeline and VertexBUffer etc have a  Bind() function that does them
    void RC_bindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>); 
    void RC_pushConstants(const std::shared_ptr<GraphicsPipeline> pipeline, const MatrixPushConstant push_constant);
    void RC_bindVertexBuffer(const std::shared_ptr<VertexBuffer> vertex_buffer);
    void RC_bindIndexBuffer(const std::shared_ptr<IndexBuffer> index_buffer);

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
    void createSemaphoresAndFences();
    void createDescriptorAllocator();
    
    VulkanFrameData& getCurrentFrame() { return m_frame_data[m_frame_count % FRAME_OVERLAP_COUNT]; };


public://VulkanRenderBackend specific public functions. might need dynamic cast

    void immediateSubmit(std::function<void(VkCommandBuffer cmd)> function);
    VkCommandBuffer getCurrentCmdBuffer() { return getCurrentFrame().m_command_buffer; };
 
 
private:
//context
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

//renderpass
    VkRenderPass                    m_render_pass;
    std::vector<VkFramebuffer>      m_framebuffers;

//swapchain
    VkSwapchainKHR                  m_swapchain;
    VkFormat                        m_swapchain_format;
    VkExtent2D                      m_swapchain_extent;
    VkPresentModeKHR                m_swapchain_present_mode;
    std::vector<VkImage>            m_swapchain_images;
    std::vector<VkImageView>        m_swapchain_views;
    uint32_t                        m_swapchain_img_idx;

//depth buffer
    VmaImage                        m_depth_image;
    VkImageView                     m_depth_image_view;
    VkFormat                        m_depth_format;

//Per frame data(command buffers, pool and sync structures)
    VulkanFrameData                 m_frame_data[FRAME_OVERLAP_COUNT];

    GPUUploadContext                m_upload_context;

    uint32_t                    m_frame_count{0};


    std::vector<const char*>    m_device_extensions;
    std::vector<const char*>    m_validation_layers;

    bool                        m_isInitialized{false};

    DeletionQueue               m_deletion_queue;

//Descriptors
    
    std::unique_ptr<DescriptorSetAllocator> m_descriptor_allocator;
    
};
