#include "pch.h"
#include "RenderGraph.h"
#include "RenderGraphPass.h"
#include "DescriptorTemplate.h"


namespace Veist
{
namespace RenderGraph
{


	RenderGraphPass::RenderGraphPass(std::string_view name, RenderGraph* graph) : m_name(name), m_graph(graph), m_resource_write_count(0), m_depth_output(nullptr) {};



	void RenderGraphPass::addDescriptorTemplate(uint32_t descriptor_set_number, uint32_t resource_index, VkDescriptorType descriptor_type, SamplerType sampler_type)
	{
		auto it = m_descriptor_set_templates.find(descriptor_set_number);
		if (it != m_descriptor_set_templates.end())
		{
			auto binding_number = it->second.size();
			it->second.emplace_back(descriptor_set_number, binding_number, resource_index, descriptor_type, sampler_type);
		}
		else
		{
			auto& desc_set = m_descriptor_set_templates.emplace(descriptor_set_number, std::vector<DescriptorTemplate>()).first->second;
			auto binding_number = desc_set.size();
			desc_set.emplace_back(descriptor_set_number, binding_number, resource_index, descriptor_type, sampler_type);
		}
	}


	void RenderGraphPass::addExternalDescriptorTemplate(uint32_t descriptor_set_number, const Descriptor& descriptor)
	{
		auto it = m_descriptor_set_templates.find(descriptor_set_number);
		if (it != m_descriptor_set_templates.end())
		{
			auto binding_number = it->second.size();
			it->second.emplace_back(descriptor_set_number, binding_number, descriptor);
		}
		else
		{
			auto& desc_set = m_descriptor_set_templates.emplace(descriptor_set_number, std::vector<DescriptorTemplate>()).first->second;
			auto binding_number = desc_set.size();
			desc_set.emplace_back(descriptor_set_number, binding_number, descriptor);
		}
	}

	ShaderBuffer* RenderGraphPass::getPhysicalBuffer(BufferResource* resource) const
	{
		if(resource->physicalResource() == nullptr)
		{
			CRITICAL_ERROR_LOG("Buffer resource does not have a phisical resource assigned to it");
		}

		return m_graph->resourcePool()->getBuffer(static_cast<PhysicalBuffer*>(resource->physicalResource()));
		 
	}

	ImageBase* RenderGraphPass::getPhysicalImage(ImageResource* resource) const
	{
		if (resource->physicalResource() == nullptr)
		{
			CRITICAL_ERROR_LOG("Image resource does not have a phisical resource assigned to it");
		}

		return m_graph->resourcePool()->getImage(static_cast<PhysicalImage*>(resource->physicalResource()));
	}


	void RenderGraphPass::executePass(CommandBuffer& cmd)
	{
		cmd.beginRenderPass(m_framebuffer);
		m_render_function(cmd, this);
		cmd.endRenderPass();
	}


	void RenderGraphPass::buildFramebuffer()
	{
		//check depth attachment is valid
		
		//Framebuffer::Attachment depth;
		std::vector<Framebuffer::Attachment> attachments;

		if (m_color_outputs.size() != m_color_inputs.size())
		{
			CRITICAL_ERROR_LOG("Rendergraph pass color outputs array does not match color inputs array");
		}

		for (int i = 0; i < m_color_outputs.size(); i++)
		{
			RenderPass::LoadOp load_op = (m_color_inputs[i] == nullptr) ? RenderPass::LoadOp::Clear : RenderPass::LoadOp::Load;
			PhysicalImage* phys_img = static_cast<PhysicalImage*>(m_color_outputs[i]->physicalResource());
			attachments.emplace_back( getPhysicalImage(m_color_outputs[i]), load_op, phys_img->imageUsageInPass(m_pass_index));
		}
		
		if (m_depth_input != nullptr || m_depth_output != nullptr)
		{
			//get physical image from either depth_input or depth_output (both should be the same physical image TODO check that in validation)
			Framebuffer::Attachment depth;
			depth.image = (m_depth_output != nullptr) ? getPhysicalImage(m_depth_output) : getPhysicalImage(m_depth_input);
			depth.load_op = (m_depth_input == nullptr) ? RenderPass::LoadOp::Clear : RenderPass::LoadOp::Load;
			
			PhysicalImage* phys_img = (m_depth_output != nullptr) ? static_cast<PhysicalImage*>(m_depth_output->physicalResource()) : static_cast<PhysicalImage*>(m_depth_input->physicalResource());

			depth.pass_usage = phys_img->imageUsageInPass(m_pass_index);

			attachments.emplace_back(depth);

		}

		m_framebuffer = Framebuffer(attachments);
		

	}

	void RenderGraphPass::buildDescriptors()
	{
	
		for (auto& desc_set_template : m_descriptor_set_templates)
		{
			std::vector<Descriptor> descriptor_bindings;
			auto& descriptor_templates = desc_set_template.second;

			int binding_num = 0;
			for (auto& d_template : descriptor_templates)
			{
				
				//check if external descriptor
				if (d_template.is_external_descriptor)
				{
					descriptor_bindings.emplace_back(*d_template.m_external_descriptor.get());
					binding_num++;
					continue;
				}

				//Create descriptor binding
				auto res_type = m_graph->getResource(d_template.m_resource_index)->resourceType();
				if (res_type == ResourceType::Image)
				{
					ImageResource* img_res = static_cast<ImageResource*>(m_graph->getResource(d_template.m_resource_index));
					ImageBase* image = getPhysicalImage(img_res);
					PhysicalImage* phys_img_res = static_cast<PhysicalImage*>(img_res->physicalResource());
					ImageUsage usage = phys_img_res->imageUsageInPass(m_pass_index);
					descriptor_bindings.emplace_back( Descriptor(d_template.m_descriptor_type, image, d_template.m_sampler_type, VK_SHADER_STAGE_ALL, usage) );
				}
				else if (res_type == ResourceType::Buffer)
				{
					ShaderBuffer* buffer = getPhysicalBuffer(static_cast<BufferResource*>(m_graph->getResource(d_template.m_resource_index)));
					descriptor_bindings.emplace_back(Descriptor(d_template.m_descriptor_type, buffer));
				}
				else
				{
					CRITICAL_ERROR_LOG("Unidentified resource type in Resource");
				}

				binding_num++;
			}

			m_descriptor_sets.emplace_back(desc_set_template.first, descriptor_bindings); //set number, bindings
		}



	}



}
}