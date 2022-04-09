#include <pch.h>
#include "Barrier.h"
#include "Veist/Graphics/Vulkan/Framebuffers/RenderPass.h"

namespace Veist
{

	static VkAccessFlags getSrcAccessMask(PipelineStage stage)
	{
		//if any shader flag, shader write
		if ((stage & PipelineStage::AnyShader) != PipelineStage::None)
		{
			return VK_ACCESS_SHADER_WRITE_BIT; 
		}

		switch (stage)
		{
			case PipelineStage::Transfer:
				return VK_ACCESS_TRANSFER_WRITE_BIT;

			case PipelineStage::ColorAttachment:
				return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			case PipelineStage::Host:
				return VK_ACCESS_HOST_WRITE_BIT;

			case PipelineStage::TopOfPipe:
			case PipelineStage::BottomOfPipe:
				return VK_ACCESS_MEMORY_WRITE_BIT;

			default:
				CRITICAL_ERROR_LOG("Pipeline stage not valid in barrier src access mask")
				break;
		}


	}

	static VkAccessFlags getDstAccessMask(PipelineStage stage)
	{
		//if any shader flag, shader write
		if ((stage & PipelineStage::AnyShader) != PipelineStage::None)
		{
			return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT; //Uniform read bit is for buffer barrier. TODO
		}

		switch (stage)
		{
		case PipelineStage::Transfer:
			return VK_ACCESS_TRANSFER_READ_BIT;

		//case PipelineStage::ColorAttachment:
			//return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		case PipelineStage::Host:
			return VK_ACCESS_HOST_READ_BIT;

		case PipelineStage::TopOfPipe:
		case PipelineStage::BottomOfPipe:
			return VK_ACCESS_MEMORY_READ_BIT;

		default:
			CRITICAL_ERROR_LOG("Pipeline stage not valid in barrier dst access mask")
			break;
		}
	}




	static VkAccessFlags getAccessFlagsFromLayout(VkImageLayout layout)
	{
		switch (layout)
		{
			case VK_IMAGE_LAYOUT_UNDEFINED:
				return 0;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				return VK_ACCESS_TRANSFER_READ_BIT;

			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				return VK_ACCESS_TRANSFER_WRITE_BIT;

			case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
				return VK_ACCESS_MEMORY_READ_BIT;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
				return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				return VK_ACCESS_SHADER_READ_BIT;

			case VK_IMAGE_LAYOUT_GENERAL:
				return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;

			default:
				CRITICAL_ERROR_LOG("Layout not supported")
				break;
		}

		
	}

	static PipelineStage getPipelineStageFromAccessFlags(VkAccessFlags access_flags)
	{

		if (access_flags == 0 || access_flags == VK_ACCESS_MEMORY_READ_BIT)
		{
			return PipelineStage::Host;
		}

		//Depth attachments
		if (access_flags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
		{
			return PipelineStage::DepthAttachmentEarly | PipelineStage::DepthAttachmentLate;
		}

		//Color attachments
		if (access_flags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT))
		{
			return PipelineStage::ColorAttachment;
		}

		//Shaders
		if (access_flags & (VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT))
		{
			return PipelineStage::AnyShader;
		}

		//Transfer
		if (access_flags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT))
		{
			return PipelineStage::Transfer;
		}
		

		CRITICAL_ERROR_LOG("Access flag not supported");

	}



	//Default arrier constructor
	ImageBarrier::ImageBarrier(const ImageBase* image, PipelineStage src, PipelineStage dst) : m_src_stage(src), m_dst_stage(dst)
	{
		m_barrier = {};
		m_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		m_barrier.srcAccessMask = getSrcAccessMask(src);
		m_barrier.dstAccessMask = getDstAccessMask(dst);
		m_barrier.image = image->image();
		m_barrier.newLayout = getImageLayout(image->imageUsage());
		m_barrier.oldLayout = getImageLayout(image->imageUsage());
		m_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		m_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		

		m_barrier.subresourceRange.aspectMask = image->properties().imageFormat().imageAspectFlags();
		m_barrier.subresourceRange.layerCount = image->properties().layerCount();
		m_barrier.subresourceRange.levelCount = image->properties().mipLevels();
	}


	//Transfer barrier constructor
	ImageBarrier::ImageBarrier(const ImageBase* image, VkImageLayout old_layout, VkImageLayout new_layout) : 
		m_src_stage(getPipelineStageFromAccessFlags(getAccessFlagsFromLayout(old_layout))),
		m_dst_stage(getPipelineStageFromAccessFlags(getAccessFlagsFromLayout(new_layout)))
	{
		
		

		m_barrier = {};
		m_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		m_barrier.srcAccessMask = getAccessFlagsFromLayout(old_layout);
		m_barrier.dstAccessMask = getAccessFlagsFromLayout(new_layout);
		m_barrier.image = image->image();
		m_barrier.newLayout = new_layout;
		m_barrier.oldLayout = old_layout;
		m_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		m_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		m_barrier.subresourceRange.aspectMask = image->properties().imageFormat().imageAspectFlags();
		m_barrier.subresourceRange.layerCount = image->properties().layerCount();
		m_barrier.subresourceRange.levelCount = image->properties().mipLevels();

	}



	ImageBarrier ImageBarrier::createTransitionBarrier(const ImageBase* image, VkImageLayout old_layout, VkImageLayout new_layout)
	{
		return ImageBarrier{ image, old_layout, new_layout};
	}



	BufferBarrier::BufferBarrier(const ShaderBuffer* buffer, PipelineStage src, PipelineStage dst) : m_src_stage(src), m_dst_stage(dst)
	{
		m_barrier = {};
		m_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		m_barrier.srcAccessMask = getSrcAccessMask(src);
		m_barrier.dstAccessMask = getDstAccessMask(dst);
		m_barrier.buffer = buffer->buffer();
		m_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		m_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		m_barrier.size = buffer->size();
		m_barrier.offset = buffer->offset();
	}


	

}