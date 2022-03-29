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

		struct Attachment
		{
			Attachment() = default;

			Attachment(ImageBase* img) : image(img)
			{}

			Attachment(ImageBase* img, LoadOp loadop) : image(img), load_op(loadop)
			{}

			ImageBase* image;
			LoadOp load_op = LoadOp::Clear;
		};


		Framebuffer() = default;
		Framebuffer(std::vector<Attachment>& colors, Attachment& depth, std::shared_ptr<RenderPass> renderpass);
		Framebuffer(std::vector<Attachment>& colors, Attachment& depth);


		//TODO: add support for framebuffer without depth attachment

		VkFramebuffer framebuffer() const {return m_framebuffer;};
		RenderPass* renderpass() const {return m_render_pass.get();};
		glm::u32vec2 size() const {return m_size;};
		size_t colorAttachmentCount() const {return m_color_attachment_count;};

	private:

		VkFramebuffer m_framebuffer;
		std::shared_ptr<RenderPass> m_render_pass;
	
		size_t m_color_attachment_count{0};
		glm::u32vec2 m_size;
	};


}