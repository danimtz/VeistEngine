/*
#include "Framebuffer.h"
#include "RenderPass.h"

#include "Engine/Renderer/RenderModule.h"


static std::shared_ptr<RenderPass> createRenderPass()
{


	return std::make_shared<RenderPass>();
}


static glm::u32vec2 calculateFramebufferSize() 
{
	uint32_t width = 0;
	uint32_t height = 0;






	return glm::u32vec2{width, height};
}


Framebuffer::Framebuffer() : m_render_pass(createRenderPass())
{

	
	auto fb_size = calculateFramebufferSize();
	
	VkFramebufferCreateInfo framebuffer_info = {};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.pNext = nullptr;

	framebuffer_info.renderPass = m_render_pass.get()->renderpass();

	framebuffer_info.width = fb_size.x;
	framebuffer_info.height = fb_size.y;
	framebuffer_info.layers = 1;

	

	VkImageView attachments[2] = { m_swapchain_views[i], m_depth_image_view };
	framebuffer_info.attachmentCount = 2;
	framebuffer_info.pAttachments = &attachments[0];


	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &m_framebuffer));

	RenderModule::getRenderBackend()->pushToDeletionQueue(	[=]() { vkDestroyFramebuffer(device, m_framebuffer, nullptr);	}
	);

}*/