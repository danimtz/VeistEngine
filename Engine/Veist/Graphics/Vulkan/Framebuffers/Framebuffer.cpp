#include "pch.h"

#include "Framebuffer.h"
#include "RenderPass.h"

#include "Veist/Graphics/RenderModule.h"


namespace Veist
{
/*
	static std::unique_ptr<RenderPass> createRenderPass(std::vector<Framebuffer::Attachment>& colors, Framebuffer::Attachment& depth)
	{
	
		std::vector<RenderPass::AttachmentProperties> color_properties;

		for (auto& attachment : colors)
		{
			color_properties.emplace_back(attachment.image->properties(), attachment.load_op, attachment.pass_usage);
		}
		RenderPass::AttachmentProperties depth_properties = { depth.image->properties(), depth.load_op, depth.pass_usage };
		return std::make_unique<RenderPass>(color_properties, depth_properties);
		
	}


	static std::unique_ptr<RenderPass> createRenderPass(std::vector<Framebuffer::Attachment>& colors)
	{
		std::vector<RenderPass::AttachmentProperties> color_properties;

		for (auto& attachment : colors)
		{
			color_properties.emplace_back(attachment.image->properties(), attachment.load_op, attachment.pass_usage);
		}
		return std::make_unique<RenderPass>(color_properties);

	}



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

		uint32_t attachment_count = (depth.pass_usage == ImageUsage::None) ? colors.size() : colors.size() + 1;

		std::vector<VkImageView> attachments;
		//add color attachment imageview to vector
		for (uint32_t i = 0; i < colors.size(); i++)
		{
			attachments.push_back(colors[i].image->imageView());
		}

		//add depth attachment imageview to vector
		if (depth.pass_usage != ImageUsage::None)
		{
			attachments.push_back(depth.image->imageView());
		}

		framebuffer_info.attachmentCount = attachment_count;
		framebuffer_info.pAttachments = attachments.data();


		VkDevice device = RenderModule::getBackend()->getDevice();

		VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer));

	

	
	}


	Framebuffer::Framebuffer(std::vector<Attachment>& colors, Attachment& depth) :
		m_render_pass(createRenderPass(colors, depth)), m_color_attachment_count(colors.size())
	{
		createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get(), m_size);
	
	}
	Framebuffer::Framebuffer(std::vector<Attachment> & colors) :
		m_render_pass(createRenderPass(colors)), m_color_attachment_count(colors.size())
	{
		Attachment empty_depth = {};
		createFramebuffer(colors, empty_depth, m_framebuffer, m_render_pass.get(), m_size);
	}


	Framebuffer::Framebuffer(std::vector<Attachment>& colors, Attachment& depth, std::shared_ptr<RenderPass> renderpass) :
		m_render_pass(renderpass), m_color_attachment_count(colors.size())
	{
		createFramebuffer(colors, depth, m_framebuffer, m_render_pass.get(), m_size);
	}
	

	*/

	static std::unique_ptr<RenderPass> createRenderPass(std::vector<Framebuffer::Attachment>& attachments)
	{

		std::vector<RenderPass::AttachmentProperties> attachment_properties;

		for (auto& attachment : attachments)
		{
			attachment_properties.emplace_back(attachment.image->properties(), attachment.load_op, attachment.pass_usage);
		}

		return std::make_unique<RenderPass>(attachment_properties);

	}



	static glm::u32vec2 calculateFramebufferSize(ImageBase* image)
	{
		uint32_t width = image->properties().imageSize().width;
		uint32_t height = image->properties().imageSize().height;

		return glm::u32vec2{ width, height };
	}


	static void createFramebuffer(std::vector<Framebuffer::Attachment>& attachments, VkFramebuffer& framebuffer, RenderPass* renderpass, glm::u32vec2& fb_size, size_t& color_attachment_count)
	{
		//TODO:check that all color attachments and depth are the same width and height

		fb_size = calculateFramebufferSize(attachments[0].image);

		color_attachment_count = attachments.size();

		VkFramebufferCreateInfo framebuffer_info = {};
		framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebuffer_info.pNext = nullptr;

		framebuffer_info.renderPass = renderpass->vk_renderpass();

		framebuffer_info.width = fb_size.x;
		framebuffer_info.height = fb_size.y;
		framebuffer_info.layers = 1;

		uint32_t attachment_count = attachments.size();

		std::vector<VkImageView> image_views;
		

		for (uint32_t i = 0; i < attachments.size(); i++)
		{
			image_views.push_back(attachments[i].image->imageView());

			//if depth attachment remove count form color attachment count
			if ((attachments[i].pass_usage & ImageUsage::DepthAttachment) != ImageUsage::None)
			{
				color_attachment_count--;
			}
		}

		framebuffer_info.attachmentCount = attachment_count;
		framebuffer_info.pAttachments = image_views.data();


		VkDevice device = RenderModule::getBackend()->getDevice();

		VK_CHECK(vkCreateFramebuffer(device, &framebuffer_info, nullptr, &framebuffer));

	}


	Framebuffer::Framebuffer(std::vector<Attachment>& attachments) :
		m_render_pass(createRenderPass(attachments))
	{
		createFramebuffer(attachments, m_framebuffer, m_render_pass.get(), m_size, m_color_attachment_count);

	}

	Framebuffer::Framebuffer(std::vector<Attachment>& attachments, std::shared_ptr<RenderPass> renderpass) :
		m_render_pass(renderpass)
	{
		createFramebuffer(attachments, m_framebuffer, m_render_pass.get(), m_size, m_color_attachment_count);
	}

	Framebuffer::~Framebuffer()
	{

		if (m_framebuffer != VK_NULL_HANDLE)
		{
			VkDevice device = RenderModule::getBackend()->getDevice();
			VkFence fence = RenderModule::getBackend()->getCurrentCmdBuffer().fence();
			VkFramebuffer framebuffer = m_framebuffer;
			RenderModule::getBackend()->registerDestruction(fence, [=]() { vkDestroyFramebuffer(device, framebuffer, nullptr);	});
		
		}

	}

	Framebuffer::Framebuffer(Framebuffer&& other)
	{
		m_framebuffer = other.m_framebuffer;
		m_render_pass = other.m_render_pass;
		m_color_attachment_count = other.m_color_attachment_count;
		m_size = other.m_size;

		other.m_framebuffer = VK_NULL_HANDLE;
		other.m_render_pass = nullptr;
	}



	//Move copy
	Framebuffer& Framebuffer::operator=(Framebuffer&& other)
	{
		if (this != &other)
		{
			// Free the existing resource if it exists
			if (m_framebuffer != VK_NULL_HANDLE)
			{
				VkDevice device = RenderModule::getBackend()->getDevice();
				vkDestroyFramebuffer(device, m_framebuffer, nullptr);
			}

			//copy resources
			m_framebuffer = other.m_framebuffer;
			m_render_pass = other.m_render_pass;
			m_color_attachment_count = other.m_color_attachment_count;
			m_size = other.m_size;

			other.m_framebuffer = VK_NULL_HANDLE;
			other.m_render_pass = nullptr;
		}
		return *this;

	}





}