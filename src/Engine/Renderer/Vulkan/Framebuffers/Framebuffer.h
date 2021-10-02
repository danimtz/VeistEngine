#pragma once


#include "RenderPass.h"
#include <vulkan/vulkan.h>


class Framebuffer
{
public:
	
	using LoadOp = RenderPass::LoadOp;

	Framebuffer() = default;
	Framebuffer(std::vector<ImageBase>& colors, ImageBase& depth, RenderPass* renderpass);
	Framebuffer(std::vector<ImageBase>& colors, ImageBase& depth, LoadOp load_op);
	
	//TODO: add support for framebuffer without depth attachment

	VkFramebuffer framebuffer() {return m_framebuffer;};


private:

	VkFramebuffer m_framebuffer;
	std::unique_ptr<RenderPass> m_render_pass;

};

