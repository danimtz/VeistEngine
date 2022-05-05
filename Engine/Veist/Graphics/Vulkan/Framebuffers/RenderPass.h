#pragma once

#include "Veist/Graphics/Vulkan/Images/Image.h"
#include <vulkan/vulkan.h>

namespace Veist
{

	VkImageLayout getImageLayout(ImageUsage usage);


	class RenderPass
	{
	public:
	
		enum class LoadOp : uint32_t {
			Load,
			Clear
		};

	


		struct AttachmentProperties {
		
			AttachmentProperties() = default;

			AttachmentProperties(const ImageProperties& props, LoadOp loadop, ImageUsage im_usage) : properties(props), load_op(loadop), usage(im_usage){};
		
			//AttachmentProperties(const ImageBase* image, LoadOp loadop) : properties(image->properties()), usage(image->imageUsage()), load_op(loadop) {};

			ImageProperties properties;
			ImageUsage usage = ImageUsage::None;
			LoadOp load_op = LoadOp::Clear;
		};

		/*struct FormatLayout
		{
			FormatLayout() : m_depth_format(VK_FORMAT_UNDEFINED) {};
			FormatLayout(const AttachmentProperties& depth_props, const std::vector<AttachmentProperties>& color_properties);

		
			//map key functions
			uint64_t hash() const;
			//bool operator()(const FormatLayout const& lhs, const FormatLayout const& rhs) const;
			bool operator <(const FormatLayout const& rhs) const;
		

			ImageFormat m_depth_format;
			std::vector<ImageFormat> m_color_formats;
		};*/

		struct FormatLayout
		{
			FormatLayout() = default;
			FormatLayout(const std::vector<AttachmentProperties>& attachment_properties);


			//map key functions
			uint64_t hash() const;
			bool operator <(const FormatLayout const& rhs) const;


			std::vector<ImageFormat> m_attachment_formats;
		};

		//RenderPass(std::vector<AttachmentProperties>& color_properties, AttachmentProperties& depth_properties);
		//RenderPass(std::vector<AttachmentProperties>& color_properties); //Renderpass without depth attachment
		
		RenderPass(std::vector<AttachmentProperties>& attachment_properties); 
		
		~RenderPass();
		RenderPass& operator=(RenderPass&& other);
		RenderPass(RenderPass&& other);
		
		//RenderPass(VkRenderPass renderpass) : m_render_pass(renderpass) {};
		//RenderPass(RenderPass* other) : m_render_pass(other->vk_renderpass()), m_format_layout(other->formatLayout()) {}; //This should really be a copy constructor

		VkRenderPass vk_renderpass() const { return m_render_pass; };
		FormatLayout formatLayout() const { return m_format_layout; };

	private:

		VkRenderPass m_render_pass{VK_NULL_HANDLE};
		FormatLayout m_format_layout;

	};



}