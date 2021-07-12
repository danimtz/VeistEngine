#pragma once

#include <GLFW/glfw3.h>
#include "Renderer/GraphicsPipeline.h"

//Vulkan abstraction class. Gonna leave it like this for now until I understand more about how vulkan works. May rewrite it completely later
class RenderBackend { 
public:

    virtual ~RenderBackend() = default;

    virtual void init(GLFWwindow* window) = 0;
    virtual void shutdown() = 0;

    virtual void* getDevice() = 0;
    virtual void* getSwapchainExtent() = 0;
    virtual void* getRenderPass() = 0;

    //Backend Commands (RC prefix -> Renderer Command) this includes all commands and functions that use graphics api specific functions
    //MAYBE SHOUDL CHANGE RC PREFIX TO GAPI or GL or something
    virtual void RC_beginFrame() = 0;
    virtual void RC_endFrame() = 0;
    virtual void RC_bindGraphicsPipeline(GraphicsPipeline *pipeline) = 0;
    virtual void RC_drawSumbit() = 0;

    static std::shared_ptr<RenderBackend> CreateBackend();
    
};
