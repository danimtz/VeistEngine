#pragma once

#include <vulkan/vulkan.h>

class Framebuffer
{
public:

	Framebuffer() = default;



	VkFramebuffer framebuffer() {return m_framebuffer;};

private:

	VkFramebuffer m_framebuffer;

	//std::unique_ptr<Renderpass> m_renderpass

};

