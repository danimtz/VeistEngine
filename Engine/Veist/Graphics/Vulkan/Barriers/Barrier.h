#pragma once

#include <vulkan/vulkan.h>
#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Buffers/ShaderBuffer.h"

namespace Veist
{

	enum class PipelineStage : uint32_t
	{
		None = 0,

		TopOfPipe = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		BottomOfPipe = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VertexInput = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VertexShader = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,

		//Tesselation control
		//Tesselation eval
		//Geometry shader
		Host = VK_PIPELINE_STAGE_HOST_BIT,

		FragmentShader = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,

		DepthAttachmentEarly =  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		DepthAttachmentLate = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,

		ColorAttachment = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		ComputeShader = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		Transfer = VK_PIPELINE_STAGE_TRANSFER_BIT,


		AnyShader = FragmentShader | VertexShader | ComputeShader,
		All = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,


	};

	inline constexpr PipelineStage operator|(PipelineStage left, PipelineStage right)
	{
		return PipelineStage(uint32_t(left) | uint32_t(right));
	}
	inline constexpr PipelineStage operator|(PipelineStage left, VkPipelineStageFlagBits  right)
	{
		return PipelineStage(uint32_t(left) | uint32_t(right));
	}

	inline constexpr PipelineStage operator&(PipelineStage left, PipelineStage right)
	{
		return PipelineStage(uint32_t(left) & uint32_t(right));
	}
	inline constexpr PipelineStage operator&(PipelineStage left, VkPipelineStageFlagBits  right)
	{
		return PipelineStage(uint32_t(left) & uint32_t(right));
	}

	inline constexpr PipelineStage operator~(PipelineStage right)
	{
		return PipelineStage(~uint32_t(right));
	}
	inline constexpr PipelineStage operator~(VkPipelineStageFlagBits  right)
	{
		return PipelineStage(~uint32_t(right));
	}





	class ImageBarrier
	{
	public:

		ImageBarrier(const ImageBase* image, PipelineStage src, PipelineStage dst);

		VkImageMemoryBarrier barrier() { return m_barrier;};
		PipelineStage dstStage() {return m_dst_stage;};
		PipelineStage srcStage() { return m_src_stage; };

	private:
	
		VkImageMemoryBarrier m_barrier;

		PipelineStage m_src_stage;
		PipelineStage m_dst_stage;

	};


	class BufferBarrier
	{
	public:

		BufferBarrier(const ShaderBuffer* buffer, PipelineStage src, PipelineStage dst);

		VkBufferMemoryBarrier barrier() { return m_barrier; };
		PipelineStage dstStage() { return m_dst_stage; };
		PipelineStage srcStage() { return m_src_stage; };

	private:

		VkBufferMemoryBarrier m_barrier;

		PipelineStage m_src_stage;
		PipelineStage m_dst_stage;

	};


}


