#pragma once


#include "RenderPass.h"
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Veist
{

class Framebuffer
{
public:
	
	using LoadOp = RenderPass::LoadOp;

	Framebuffer() = default;
	Framebuffer(std::vector<ImageBase*>& colors, ImageBase* depth, RenderPass* renderpass);
	Framebuffer(std::vector<ImageBase*>& colors, ImageBase* depth, LoadOp load_op);

	Framebuffer(ImageBase* color_attachment, ImageBase* depth, LoadOp load_op);

	//TODO: add support for framebuffer without depth attachment

	VkFramebuffer framebuffer() const {return m_framebuffer;};
	RenderPass* renderpass() const {return m_render_pass.get();};
	glm::u32vec2 size() const {return m_size;};
	size_t colorAttachmentCount() const {return m_color_attachment_count;};

private:

	VkFramebuffer m_framebuffer;
	std::unique_ptr<RenderPass> m_render_pass;
	
	size_t m_color_attachment_count;
	glm::u32vec2 m_size;
};


}