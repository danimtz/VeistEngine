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