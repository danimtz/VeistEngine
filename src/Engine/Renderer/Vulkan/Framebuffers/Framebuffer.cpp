
#include "Framebuffer.h"
#include "RenderPass.h"

#include "Engine/Renderer/RenderModule.h"


static std::unique_ptr<RenderPass> createRenderPass(std::vector<ImageBase>& colors, ImageBase& depth, RenderPass::LoadOp load_op)
{
	
	std::vector<RenderPass::AttachmentProperties> color_properties;

	for (uint32_t i = 0; i < colors.size(); i++)
	{
		color_properties.push_back({colors[i], load_op});
	}

	RenderPass::AttachmentProperties depth_properties = {depth, load_op};

	return std::make_unique<RenderPass>(color_properties, depth_properties);
}


//Creates a RenderPass that links ot an existing VkRenderPass (used for swapchain framebuffer)
static std::unique_ptr<RenderPass> setExistingRenderPass(VkRenderPass renderpass)
{
	return std::make_unique<RenderPass>(renderpass);
}


static glm::u32vec2 calculateFramebufferSize(ImageBase& image)
{
	uint32_t width = image.properties().imageSize().width;
	uint32_t height = image.properties().imageSize().height;

	return glm::u32vec2{width, height};
}


static void createFramebuffer(std::vector<ImageBase>& colors, ImageBase& depth, VkFramebuffer& framebuffer, RenderPass* renderpass)
{
	//TODO:check that all color attachments and depth are the same width and height
	
	auto fb_size = calculateFramebufferSize(colors[0]);

	VkFramebufferCreateInfo framebuffer_info = {};
	framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_info.pNext = nullptr;

	framebuffer_info.renderPass = renderpass->renderpass();

	framebuffer_info.width = fb_size.x;
	framebuffer_info.height = fb_size.y;
	framebuffer_info.layers = 1;

	uint32_t attachment_count = colors.size() + 1;

	std::vector<VkImageView> attachments;
	//add color attachment imageview to vector
	for (uint32_t i = 0; i < colors.size(); i++)
	{
		attachments.push_back(colors[i].imageView());
	}
	//add depth attachment imageview to vector
	attachments.push_back(depth.imageView());

	framebuffer_info.attachmentCount = attachment_count;
	framebuffer_info.pAttachments = attachments.data();


	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer));

	
	if ((colors[0].imageUsage() & ImageUsage::SwapchainImage) != ImageUsage::None)
	{
		RenderModule::getRenderBackend()->pushToSwapchainDeletionQueue([=]() { vkDestroyFramebuffer(device, framebuffer, nullptr);	});
	}
	else 
	{
		RenderModule::getRenderBackend()->pushToDeletionQueue([=]() { vkDestroyFramebuffer(device, framebuffer, nullptr);	});
	}

	
	
}


Framebuffer::Framebuffer(std::vector<ImageBase>& colors, ImageBase& depth, LoadOp load_op) : m_render_pass(createRenderPass(colors, depth, load_op))
{
	createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get());
}


Framebuffer::Framebuffer(std::vector<ImageBase>& colors, ImageBase& depth, RenderPass* renderpass) : m_render_pass(std::make_unique<RenderPass>(renderpass->renderpass()))
{
	createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get());
}