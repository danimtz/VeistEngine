#pragma once

class RenderBackend { 
public:

    virtual ~RenderBackend() = default;

    virtual void init() = 0;
    virtual void shutdown() = 0;

    //Backend Commands (RC prefix -> Renderer Command) this includes all commands and functions that use graphics api specific functions
    //MAYBE SHOUDL CHANGE RC PREFIX TO GAPI or GL or something
    virtual void RC_beginFrame() = 0;
    virtual void RC_endFrame() = 0;

    static std::unique_ptr<RenderBackend> CreateBackend(void* window);
    
};
