#pragma once

//#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
//#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Descriptors/DescriptorSet.h"

#include "Veist/Util/AssetLoader.h"

#include "Veist/Material/MaterialType.h"
#include "Veist/Material/MaterialData.h"

namespace Veist
{

/*
class Material
{
public:

	Material(const char* material_name, const VertexDescription& vertex_desc);
	virtual ~Material() = default;

	const DescriptorSet& descriptorSet() const { return m_descriptor_set; };
	const GraphicsPipelineBuilder* pipelineBuilder() const {return m_pipeline_builder.get();};
	
	//Build pipeline based on renderpass, if it already exists return the pipeline
	GraphicsPipeline* getPipeline(const RenderPass* renderpass);

protected:
	
	//Configures the descriptor set for the material should be called from constructor of subclass
	virtual void setUpDescriptorSet() = 0;

	std::unique_ptr<GraphicsPipelineBuilder> m_pipeline_builder;

	std::map<RenderPass::FormatLayout, GraphicsPipeline> m_compiled_pipelines;


	DescriptorSet m_descriptor_set;
	static const uint32_t MATERIAL_DESCRIPTOR_SET_NUMBER = 1;
	static const uint32_t MAX_PIPELINES_PER_MATERIAL = 4;
};
*/

	class Material
	{
	public:
		
		Material(MaterialType* type, MaterialData data);
		~Material() = default;

		const DescriptorSet& descriptorSet() const { return m_descriptor_set; };

		MaterialType* materialType() const { return m_material_type; };

	private:

		MaterialType* m_material_type;

		MaterialData m_material_data;

		DescriptorSet m_descriptor_set;
	};


}