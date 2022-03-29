#include "pch.h"

#include "Framebuffer.h"
#include "RenderPass.h"

#include "Veist/Graphics/RenderModule.h"


namespace Veist
{

	static std::unique_ptr<RenderPass> createRenderPass(std::vector<Framebuffer::Attachment>& colors, Framebuffer::Attachment& depth)
	{
	
		std::vector<RenderPass::AttachmentProperties> color_properties;

		for (auto& attachment : colors)
		{
			color_properties.emplace_back(attachment.image, attachment.load_op);
		}

		RenderPass::AttachmentProperties depth_properties = {depth.image, depth.load_op};

		return std::make_unique<RenderPass>(color_properties, depth_properties);
	}


	//Creates a RenderPass that links ot an existing VkRenderPass (used for swapchain framebuffer)
	/*static std::unique_ptr<RenderPass> setExistingRenderPass(VkRenderPass renderpass)
	{
		return std::make_unique<RenderPass>(renderpass);
	}*/


	static glm::u32vec2 calculateFramebufferSize(ImageBase* image)
	{
		uint32_t width = image->properties().imageSize().width;
		uint32_t height = image->properties().imageSize().height;

		return glm::u32vec2{width, height};
	}


	static void createFramebuffer(std::vector<Framebuffer::Attachment>& colors, Framebuffer::Attachment& depth, VkFramebuffer& framebuffer, RenderPass* renderpass, glm::u32vec2& fb_size)
	{
		//TODO:check that all color attachments and depth are the same width and height
	
		fb_size = calculateFramebufferSize(colors[0].image);

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.pNext = nullptr;

		framebuffer_info.renderPass = renderpass->vk_renderpass();

		framebuffer_info.width = fb_size.x;
		framebuffer_info.height = fb_size.y;
		framebuffer_info.layers = 1;

		uint32_t attachment_count = colors.size() + 1;

		std::vector<VkImageView> attachments;
		//add color attachment imageview to vector
		for (uint32_t i = 0; i < colors.size(); i++)
		{
			attachments.push_back(colors[i].image->imageView());
		}
		//add depth attachment imageview to vector
		attachments.push_back(depth.image->imageView());

		framebuffer_info.attachmentCount = attachment_count;
		framebuffer_info.pAttachments = attachments.data();


		VkDevice device = RenderModule::getBackend()->getDevice();
		VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer));

	
		if ((colors[0].image->imageUsage() & ImageUsage::SwapchainImage) != ImageUsage::None)
		{
			RenderModule::getBackend()->pushToSwapchainDeletionQueue([=]() { vkDestroyFramebuffer(device, framebuffer, nullptr);	});
		}
		else 
		{
			RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroyFramebuffer(device, framebuffer, nullptr);	});
		}

	
	
	}


	Framebuffer::Framebuffer(std::vector<Attachment>& colors, Attachment& depth) :
		m_render_pass(createRenderPass(colors, depth)), m_color_attachment_count(colors.size())
	{
		createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get(), m_size);
	}


	Framebuffer::Framebuffer(std::vector<Attachment>& colors, Attachment& depth, std::shared_ptr<RenderPass> renderpass) :
		m_render_pass(renderpass), m_color_attachment_count(colors.size())
	{
		createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get(), m_size);
	}




}