#pragma once

#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"

namespace Veist
{


	class MaterialType
	{
	public:
		
		MaterialType(const char* material_name, const VertexDescription& vertex_desc, uint32_t attachment_count = 1, uint32_t descriptor_set_number = 1, DepthTest depth = DepthTest::ReadWrite) :
			m_descriptor_set_number(descriptor_set_number), m_pipeline_builder(std::make_shared<GraphicsPipelineBuilder>(material_name, vertex_desc, attachment_count, depth)) {}
		//MaterialType() = default;
		~MaterialType() = default;

		const GraphicsPipelineBuilder* pipelineBuilder() const { return m_pipeline_builder.get(); };

		//Build pipeline based on renderpass, if it already exists return the pipeline
		GraphicsPipeline* getPipeline(const RenderPass* renderpass);

		uint32_t descSetNumber() const {return m_descriptor_set_number;};

	protected:

		std::shared_ptr<GraphicsPipelineBuilder> m_pipeline_builder;
		std::map<RenderPass::FormatLayout, GraphicsPipeline> m_compiled_pipelines;
		const uint32_t m_descriptor_set_number{1};

		static const uint32_t MAX_PIPELINES_PER_MATERIAL = 4;
	};


};
	
