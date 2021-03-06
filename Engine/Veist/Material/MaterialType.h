#pragma once

#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"

namespace Veist
{

	struct MaterialSettings
	{

		VertexDescription vertex_description;
		uint32_t descriptor_set_number{0};
		uint32_t attachment_count;
		std::string vertex_shader_name;
		std::string fragment_shader_name;
		DepthTest depth_setting;
		PolygonMode polygon_mode{PolygonMode::Fill};
		CullMode cull_mode{CullMode::Back};

	};

	static const uint32_t MATERIAL_DESCRIPTOR_NUMBER = 1; //TODO: This feels hacky. maybe rework it

	class MaterialType
	{
	public:

		MaterialType(const MaterialSettings& settings);
		/*MaterialType(const char* material_name, const VertexDescription& vertex_desc, uint32_t attachment_count = 1, uint32_t descriptor_set_number = 1, DepthTest depth = DepthTest::ReadWrite) :
			m_descriptor_set_number(descriptor_set_number), m_pipeline_builder(std::make_shared<GraphicsPipelineBuilder>(material_name, vertex_desc, attachment_count, depth)) {}
		*/
		//MaterialType() = default;
		~MaterialType() = default;

		const GraphicsPipelineBuilder* pipelineBuilder() const { return m_pipeline_builder.get(); };

		//Build pipeline based on renderpass, if it already exists return the pipeline
		GraphicsPipeline* getPipeline(const RenderPass* renderpass);

		uint32_t descSetNumber() const { return m_descriptor_set_number; };

	protected:

		std::shared_ptr<GraphicsPipelineBuilder> m_pipeline_builder;
		std::map<RenderPass::FormatLayout, GraphicsPipeline> m_compiled_pipelines;
		const uint32_t m_descriptor_set_number;

		static const uint32_t MAX_PIPELINES_PER_MATERIAL = 4;
	};


};
