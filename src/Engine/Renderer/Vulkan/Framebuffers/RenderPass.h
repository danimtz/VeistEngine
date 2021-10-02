#pragma once

#include "Engine/Renderer/Vulkan/Images/Image.h"
#include <vulkan/vulkan.h>

class RenderPass
{
public:
	
	enum class LoadOp : uint32_t {
		Load,
		Clear
	};

	struct AttachmentProperties {
		
		AttachmentProperties() = default;

		AttachmentProperties(ImageProperties& props, LoadOp loadop, ImageUsage im_usage) : properties(props), load_op(loadop), usage(im_usage){};
		
		AttachmentProperties(const ImageBase& image, LoadOp loadop) : properties(image.properties()), usage(image.imageUsage()), load_op(loadop) {};

		ImageProperties properties;
		ImageUsage usage = ImageUsage::None;
		LoadOp load_op = LoadOp::Clear;
	};


	
	
	RenderPass(std::vector<AttachmentProperties>& color_properties); //Renderpass without depth attachment
	RenderPass(std::vector<AttachmentProperties>& color_properties, AttachmentProperties& depth_properties);
	RenderPass(VkRenderPass renderpass) : m_render_pass(renderpass) {};
	RenderPass() = default;

	VkRenderPass renderpass() const { return m_render_pass; };
	
private:

	VkRenderPass m_render_pass;
};

