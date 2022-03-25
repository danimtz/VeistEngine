#include "pch.h"

#include "Material.h"

namespace Veist
{
/*

	Material::Material(const char* material_name, const VertexDescription& vertex_desc)
	{
		m_pipeline_builder = std::make_unique<GraphicsPipelineBuilder>(material_name, vertex_desc);
	}

	GraphicsPipeline* Material::getPipeline(const RenderPass* renderpass)
	{
		
		auto& it = m_compiled_pipelines.find(renderpass->formatLayout());
		if (it != m_compiled_pipelines.end())
		{
			return &it->second;
		}
		else
		{
			//If more than x amount of pipelines are already compiled remove one of them and add the new one
			if (m_compiled_pipelines.size() > MAX_PIPELINES_PER_MATERIAL)
			{
				m_compiled_pipelines.erase(m_compiled_pipelines.begin());
			}

			auto& pipeline = m_compiled_pipelines.emplace(renderpass->formatLayout(), m_pipeline_builder->buildPipeline(renderpass)).first->second;
			return &pipeline;
		
		}



	}
	*/
	

	Material::Material(MaterialType* type, MaterialData data) : m_material_type(type), m_material_data(data)
	{

		m_descriptor_set.setDescriptorSetLayout( type->descSetNumber(), type->pipelineBuilder());

		uint32_t count = 0;
		for (auto& image : data.textures())
		{
			m_descriptor_set.bindCombinedSamplerImage(count, image.get(), { SamplerType::RepeatLinear });
			count++;
		}

		m_descriptor_set.buildDescriptorSet();

	}
	

}