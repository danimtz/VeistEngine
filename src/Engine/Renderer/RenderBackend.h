#pragma once



#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Buffers/Buffer.h"
#include <GLFW/glfw3.h>
#include <functional>
#include <memory>

//Vulkan abstraction class. Gonna leave it like this for now until I understand more about how vulkan works. May rewrite it completely later
class RenderBackend { 
public:

    virtual ~RenderBackend() = default;

    virtual void init(GLFWwindow* window) = 0;
    virtual void shutdown() = 0;
    virtual void pushToDeletionQueue(std::function<void()> function) = 0;

    virtual GLFWwindow* getWindow() const = 0;
    virtual void* getDevice() const = 0;
    virtual void* getSwapchainExtent() = 0;
    virtual void* getRenderPass() const = 0;
    virtual void* getAllocator() const = 0;
    virtual uint32_t getFrameNumber() const = 0;

    //Backend Commands (RC prefix -> Renderer Command) this includes all commands and functions that use graphics api specific functions
    //MAYBE SHOUDL CHANGE RC PREFIX TO GAPI or GL or something
    virtual void RC_beginFrame() = 0;
    virtual void RC_endFrame() = 0;
    virtual void RC_bindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline> pipeline) = 0;
    virtual void RC_pushConstants(const std::shared_ptr<GraphicsPipeline> pipeline, const MatrixPushConstant push_constant) = 0;
    virtual void RC_bindVertexBuffer(const std::shared_ptr<VertexBuffer> vertex_buffer) = 0;
    virtual void RC_bindIndexBuffer(const std::shared_ptr<IndexBuffer> index_buffer) = 0;

    virtual void RC_drawIndexed(uint32_t size) = 0;
    virtual void RC_drawSumbit(uint32_t size) = 0;

    static std::shared_ptr<RenderBackend> CreateBackend();
    
};
