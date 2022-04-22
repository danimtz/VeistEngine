#include "pch.h"
#include "PassBuilder.h"
#include "RenderGraph.h"


namespace Veist
{
namespace RenderGraph
{





	void PassBuilder::setRenderFunction(RenderFunction&& function)
	{
		m_graph_pass->m_render_function = std::move(function);
	}


	void PassBuilder::setRenderGraphBackbuffer(const std::string& name)
	{
		m_graph_pass->m_graph->setBackbuffer(name);
	}

	void PassBuilder::setRenderGraphImGuiBackbuffer(const std::string& name)
	{
		auto img_res = m_graph_pass->m_graph->getImageResource(name);
		img_res->addImageUsage(ImageUsage::Texture, m_graph_pass->m_pass_index);
		m_graph_pass->m_graph->setBackbuffer(name);
	}


	ImageResource* PassBuilder::addImageToPass(ResourceAction action, const std::string& name, const ImageInfo& info, PipelineStage stage, ImageUsage usage)
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
		
		//Stage
		image_res->addStage(stage, m_graph_pass->m_pass_index);

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

		image_res->addImageUsage(usage, m_graph_pass->m_pass_index);

		m_graph_pass->m_pass_resources.emplace(image_res);

		return image_res;
	}

	BufferResource* PassBuilder::addBufferToPass(ResourceAction action, const std::string& name, const BufferInfo& info, PipelineStage stage, ShaderBufferUsage usage)
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

		//stage
		buffer_res->addStage(stage, m_graph_pass->m_pass_index);

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

		m_graph_pass->m_pass_resources.emplace(buffer_res);

		return buffer_res;
	}


	//============== Render graph pass reads/inputs ===================//

	
	BufferResource* PassBuilder::addUniformInput(const std::string& name, const BufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Uniform);
		
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		return buffer_res;

	}
	BufferResource* PassBuilder::addUniformInput(const std::string& name, PipelineStage stage, uint32_t d_set_index)
	{
		BufferInfo NOT_USED;
		return addUniformInput(name, NOT_USED, stage, d_set_index);
	}


	BufferResource* PassBuilder::addStorageInput(const std::string& name, const BufferInfo& info, PipelineStage stage, uint32_t d_set_index)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Read, name, info, stage, ShaderBufferUsage::Storage);
		m_graph_pass->addDescriptorTemplate(d_set_index, buffer_res->index(), VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
		return buffer_res;
	}
	BufferResource* PassBuilder::addStorageInput(const std::string& name, PipelineStage stage, uint32_t d_set_index)
	{
		BufferInfo NOT_USED;
		return addStorageInput(name, NOT_USED, stage, d_set_index);
	}


	/* TODO: For tile based rendering, input attachments are more efficient. not implemented
	ImageResource* PassBuilder::addAttachmentInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage,  uint32_t d_set_index)
	{
		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, (ImageUsage::ColorAttachment));
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, sampler_type);
		return image_res;
	}*/
	
	ImageResource* PassBuilder::addTextureInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index)
	{
		auto* image_res = addImageToPass(ResourceAction::Read, name, info, stage, ImageUsage::Texture);
		m_graph_pass->addDescriptorTemplate(d_set_index, image_res->index(), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler_type);
		return image_res;
	}
	ImageResource* PassBuilder::addTextureInput(const std::string& name, SamplerType sampler_type, 
		PipelineStage stage, uint32_t d_set_index )
	{
		ImageInfo NOT_USED;
		return addTextureInput(name, NOT_USED, sampler_type, stage, d_set_index);
	}


	ImageResource* PassBuilder::addDepthInput(const std::string& name, const ImageInfo& info, SamplerType sampler_type,
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
	ImageResource* PassBuilder::addDepthInput(const std::string& name, SamplerType sampler_type,
		PipelineStage stage, uint32_t d_set_index)
	{
		ImageInfo NOT_USED;
		return addDepthInput(name, NOT_USED, sampler_type, stage, d_set_index);
	}



	void PassBuilder::addExternalInput(const std::string& name, Descriptor descriptor, const uint32_t d_set_index)
	{

		m_graph_pass->addExternalDescriptorTemplate(d_set_index, descriptor);

	}


	//============== Render graph pass writes/output ===================//

	ImageResource* PassBuilder::addColorOutput(const std::string& name, const ImageInfo& info, const std::string& input)
	{
		//texture as well so that it can be sampled TODO remove when editor pass is created i guess
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, PipelineStage::ColorAttachment, ImageUsage::ColorAttachment);
		m_graph_pass->m_color_outputs.emplace_back(image_res);

		if (!input.empty())
		{
			auto* input_res = addImageToPass(ResourceAction::Read, input, info, PipelineStage::ColorAttachment, ImageUsage::ColorAttachment);// m_graph_pass->m_graph->addImageResource(name).second;
			m_graph_pass->m_color_inputs.emplace_back(input_res);

			//Alias Read/Modify/Write resource
			//auto* graph = m_graph_pass->m_graph;
			//image_res->aliasImageResource(input_res);
			//graph->m_resource_to_idx_map[input] = graph->m_resource_to_idx_map[name];


		}
		else
		{
			m_graph_pass->m_color_inputs.emplace_back(nullptr);
		}


		return image_res;
	}


	ImageResource* PassBuilder::addDepthOutput(const std::string& name, const ImageInfo& info, const std::string& input )
	{
		//Only 1 depth image per renderpass
		if (m_graph_pass->m_depth_output != nullptr)
		{
			CRITICAL_ERROR_LOG("RenderGraphPass attempting to write multiple depth outputs to pass: " + name);
		}
		
		
		//TODO decide on stage for depth attachment
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, (PipelineStage::DepthAttachment), ImageUsage::DepthAttachment);
		m_graph_pass->m_depth_output = image_res;
		return image_res;

		if (!input.empty())
		{
			auto* input_res = addImageToPass(ResourceAction::Read, input, info, PipelineStage::ColorAttachment, ImageUsage::ColorAttachment);// m_graph_pass->m_graph->addImageResource(name).second;
			m_graph_pass->m_depth_input = input_res;

			//Alias Read/Modify/Write resource
			//auto* graph = m_graph_pass->m_graph;
			//image_res->aliasImageResource(input_res);
			//graph->m_resource_to_idx_map[input] = graph->m_resource_to_idx_map[name];

		}


	}


	ImageResource* PassBuilder::addStorageTextureOutput(const std::string& name, const ImageInfo& info, const std::string& input, PipelineStage stage)
	{
		auto* image_res = addImageToPass(ResourceAction::Write, name, info, stage, ImageUsage::Storage);
		return image_res;
	}


	BufferResource* PassBuilder::addStorageOutput(const std::string& name, const BufferInfo& info, const std::string& input, PipelineStage stage)
	{
		auto* buffer_res = addBufferToPass(ResourceAction::Write, name, info, stage, ShaderBufferUsage::Storage);
		return buffer_res;
	}


}
}