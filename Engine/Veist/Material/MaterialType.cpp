#include "pch.h"

#include "MaterialType.h"


namespace Veist
{


	MaterialType::MaterialType(const MaterialSettings& settings) :
		m_descriptor_set_number(settings.descriptor_set_number), m_pipeline_builder(std::make_shared<GraphicsPipelineBuilder>(settings))
	{
	}

	GraphicsPipeline* MaterialType::getPipeline(const RenderPass* renderpass)
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


}