#include "pch.h"
#include "RenderGraphPassBuilder.h"
#include "RenderGraph.h"


namespace Veist
{




	void RenderGraphPassBuilder::setRenderFunction(RenderFunction&& function)
	{
		m_graph_pass->m_render_function = std::move(function);
	}


	RenderGraphImageResource* RenderGraphPassBuilder::addImageToPass(ResourceAction action, const std::string& name, const RenderGraphImageInfo& info, PipelineStage stage, ImageUsage usage)
	{
		//Get resource
		auto ret = m_graph_pass->m_graph->addImageResource(name);
		auto found = ret.first;
		auto* image_res = ret.second;
		if (action == ResourceAction::Read)
		{
			m_graph_pass->m_resource_reads.emplace_back(image_res);
			image_res->setReadInPass(m_graph_pass->m_pass_index);
		}
		else if (action == ResourceAction::Write)
		{
			m_graph_pass->m_resource_writes.emplace_back(image_res);
			image_res->setWrittenInPass(m_graph_pass->m_pass_index);
			m_graph_pass->m_resource_write_count++;
		}
		
		//TODO Stage

		//If image name was found and not newly addded
		if (!found)
		{
			//check that image info is not empt
			if (info.properties.imageSize().n_channels == 0)
			{
				CRITICAL_ERROR_LOG("Resource being added for first time without valid resource info: " + name);
			}

			image_res->setImageInfo(info);
			image_res->setResourceName(name);
		}

		image_res->addImageUsage(usage);

		return image_res;
	}

	RenderGraphBufferResource* RenderGraphPassBuilder::addBufferToPass(ResourceAction action, const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, ShaderBufferUsage usage)
	{
		//Get resource
		auto ret = m_graph_pass->m_graph->addBufferResource(name);
		auto found = ret.first;
		auto* buffer_res = ret.second;

		if (action == ResourceAction::Read)
		{
			m_graph_pass->m_resource_reads.emplace_back(buffer_res);
			buffer_res->setReadInPass(m_graph_pass->m_pass_index);
		}
		else if (action == ResourceAction::Write)
		{
			m_graph_pass->m_resource_writes.emplace_back(buffer_res);
			buffer_res->setWrittenInPass(m_graph_pass->m_pass_index);
			m_graph_pass->m_resource_write_count++;
		}

		//TODO stage

		//If buffer name was found and not newly addded
		if (!found)
		{
			//check that image info is not empt
			if (info.size == 0)
			{
				CRITICAL_ERROR_LOG("Resource being added for first time without valid resource info: " + name);
			}

			buffer_res->setBufferInfo(info);
			buffer_res->setResourceName(name);
		}

		buffer_res->setBufferUsage(usage);
		return buffer_res;
	}


	//============== Render graph pass reads/inputs ===================//

	
	RenderGraphBufferResource* RenderGraphPassBuilder::addUniformInput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Uniform);
		
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		return buffer_res;

	}
	RenderGraphBufferResource* RenderGraphPassBuilder::addUniformInput(const std::string& name, PipelineStage stage, uint32_t d_set_index)
	{
		RenderGraphBufferInfo NOT_USED;
		return addUniformInput(name, NOT_USED, stage, d_set_index);
	}


	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageInput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Storage);
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		return buffer_res;
	}
	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageInput(const std::string& name, PipelineStage stage, uint32_t d_set_index)
	{
		RenderGraphBufferInfo NOT_USED;
		return addStorageInput(name, NOT_USED, stage, d_set_index);
	}


	/* TODO: For tile based rendering, input attachments are more efficient. not implemented
	RenderGraphImageResource* RenderGraphPassBuilder::addAttachmentInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage,  uint32_t d_set_index)
	{
		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, (ImageUsage::ColorAttachment));
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, sampler_type);
		return image_res;
	}*/
	
	RenderGraphImageResource* RenderGraphPassBuilder::addTextureInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index)
	{
		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::Texture);
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		return image_res;
	}
	RenderGraphImageResource* RenderGraphPassBuilder::addTextureInput(const std::string& name, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index )
	{
		RenderGraphImageInfo NOT_USED;
		return addTextureInput(name, NOT_USED, sampler_type, stage, d_set_index);
	}


	RenderGraphImageResource* RenderGraphPassBuilder::addDepthInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type,
		PipelineStage stage, uint32_t d_set_index)
	{
		//Only 1 depth image per renderpass
		if (m_graph_pass->m_depth_input != nullptr)
		{
			CRITICAL_ERROR_LOG("RenderGraphPass attempting to read multiple depth inputs in pass: " + name);
		}

		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::DepthAttachment);
		//m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		m_graph_pass->m_depth_input = image_res;
		return image_res;
	}
	RenderGraphImageResource* RenderGraphPassBuilder::addDepthInput(const std::string& name, SamplerType sampler_type,
		PipelineStage stage, uint32_t d_set_index)
	{
		RenderGraphImageInfo NOT_USED;
		return addDepthInput(name, NOT_USED, sampler_type, stage, d_set_index);
	}



	void RenderGraphPassBuilder::addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index)
	{

		m_graph_pass->addExternalDescriptorTemplate(d_set_index, descriptor);

	}


	//============== Render graph pass writes/output ===================//

	RenderGraphImageResource* RenderGraphPassBuilder::addColorOutput(const std::string& name, const RenderGraphImageInfo& info, const std::string& input)
	{
		//texture as well so that it can be sampled TODO remove when editor pass is created i guess
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, PipelineStage::ColorAttachment, (ImageUsage::ColorAttachment | ImageUsage::Texture));
		m_graph_pass->m_color_outputs.emplace_back(image_res);

		if (!input.empty())
		{
			auto* input_res = m_graph_pass->m_graph->addImageResource(name).second;
			m_graph_pass->m_color_inputs.emplace_back(input_res);
		}
		else
		{
			m_graph_pass->m_color_inputs.emplace_back(nullptr);
		}


		return image_res;
	}
	/*RenderGraphImageResource* RenderGraphPassBuilder::addColorOutput(const std::string& name)
	{
		//texture as well so that it can be sampled TODO remove when editor pass is created i guess
		RenderGraphImageInfo NOT_USED;
		return addColorOutput(name, NOT_USED);
	}*/


	RenderGraphImageResource* RenderGraphPassBuilder::addDepthOutput(const std::string& name, const RenderGraphImageInfo& info)
	{
		//Only 1 depth image per renderpass
		if (m_graph_pass->m_depth_output != nullptr)
		{
			CRITICAL_ERROR_LOG("RenderGraphPass attempting to write multiple depth outputs to pass: " + name);
		}
		

		//TODO decide on stage for depth attachment
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, (PipelineStage::DepthAttachmentEarly | PipelineStage::DepthAttachmentLate), ImageUsage::DepthAttachment);
		m_graph_pass->m_depth_output = image_res;
		return image_res;
	}
	/*
	RenderGraphImageResource* RenderGraphPassBuilder::addDepthOutput(const std::string& name)
	{
		RenderGraphImageInfo NOT_USED;
		return addDepthOutput(name, NOT_USED);
	}*/


	RenderGraphImageResource* RenderGraphPassBuilder::addStorageTextureOutput(const std::string& name, const RenderGraphImageInfo& info, const std::string& input, PipelineStage stage)
	{
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, stage, ImageUsage::Storage);
		return image_res;
	}
	/*
	RenderGraphImageResource* RenderGraphPassBuilder::addStorageTextureOutput(const std::string& name, PipelineStage stage)
	{
		RenderGraphImageInfo NOT_USED;
		return addStorageTextureOutput(name, NOT_USED, stage);
	}*/


	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageOutput(const std::string& name, const RenderGraphBufferInfo& info, const std::string& input, PipelineStage stage)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Write, name, info, stage, ShaderBufferUsage::Storage);
		return buffer_res;
	}

	/*
	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageOutput(const std::string& name, PipelineStage stage)
	{

		RenderGraphBufferInfo NOT_USED;
		return addStorageOutput(name, NOT_USED, stage);

	}*/


}