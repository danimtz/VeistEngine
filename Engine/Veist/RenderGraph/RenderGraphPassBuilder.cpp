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
		auto* image_res = m_graph_pass->m_graph->getOrAddImageResource(name);

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

		//fill in resource info
		image_res->setImageInfo(info);
		image_res->setResourceName(name);
		image_res->setImageUsage(usage);

		return image_res;
	}

	RenderGraphBufferResource* RenderGraphPassBuilder::addBufferToPass(ResourceAction action, const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, ShaderBufferUsage usage)
	{
		//Get resource
		auto* buffer_res = m_graph_pass->m_graph->getOrAddBufferResource(name);
		
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


		//fill in resource info
		buffer_res->setBufferInfo(info);
		buffer_res->setResourceName(name);
		buffer_res->setBufferUsage(usage);
		return buffer_res;
	}


	//============== Render graph pass reads/inputs ===================//

	//TODO: if getOrAddBufferResource(name) returns an already existing resource, i shouldnt re add the name, info etc etc, i should  only do the m_graph_pass-> operations

	RenderGraphBufferResource* RenderGraphPassBuilder::addUniformInput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Uniform);
		
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		return buffer_res;

	}


	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageInput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{

		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Storage);
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		return buffer_res;
	}


	RenderGraphImageResource* RenderGraphPassBuilder::addAttachmentInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage,  uint32_t d_set_index)
	{
		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::ColorAttachment);
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		return image_res;
	}
	
	RenderGraphImageResource* RenderGraphPassBuilder::addTextureInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index )
	{

		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::Texture);
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		return image_res;
	}



	RenderGraphImageResource* RenderGraphPassBuilder::addDepthInput(const std::string& name, const RenderGraphImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index)
	{

		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::DepthAttachment);
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		return image_res;
	}


	void RenderGraphPassBuilder::addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index)
	{

		m_graph_pass->addExternalDescriptorTemplate(d_set_index, descriptor);

	}


	//============== Render graph pass writes/output ===================//

	RenderGraphImageResource* RenderGraphPassBuilder::addColorOutput(const std::string& name, const RenderGraphImageInfo& info)
	{
	

		//texture as well so that it can be sampled TODO remove when editor pass is created i guess
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, PipelineStage::ColorAttachment, (ImageUsage::ColorAttachment | ImageUsage::Texture));
		m_graph_pass->m_color_outputs.emplace_back(image_res);
		return image_res;
	}


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


	RenderGraphImageResource* RenderGraphPassBuilder::addStorageTextureOutput(const std::string& name, const RenderGraphImageInfo& info, PipelineStage stage)
	{

		auto* image_res = addImageToPass(ResourceAction::Write, name, info, stage, ImageUsage::Storage);
		return image_res;
	}


	RenderGraphBufferResource* RenderGraphPassBuilder::addStorageOutput(const std::string& name, const RenderGraphBufferInfo& info, PipelineStage stage)
	{
	
		
		auto* buffer_res = addBufferToPass(ResourceAction::Write, name, info, stage, ShaderBufferUsage::Storage);
		return buffer_res;

	}


}