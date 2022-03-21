#include "pch.h"

#include "SkyboxMaterial.h"


namespace Veist
{

SkyboxMaterial::SkyboxMaterial(const char* material_name, const VertexDescription& vertex_desc, std::shared_ptr<Cubemap> cubemap) : m_cubemap(cubemap)
{

	createPipeline(material_name, vertex_desc);
	setUpDescriptorSet();

}



void SkyboxMaterial::setUpDescriptorSet()
{

	m_descriptor_set.setDescriptorSetLayout(SKYBOX_MATERIAL_DESCRIPTOR_SET_NUMBER, m_pipeline.get());


	m_descriptor_set.bindCombinedSamplerImage(0, m_cubemap.get(), { SamplerType::RepeatLinear });
	

	m_descriptor_set.buildDescriptorSet();
}


void SkyboxMaterial::createPipeline(const char* material_name, const VertexDescription& vertex_desc) {

	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc);

}

}