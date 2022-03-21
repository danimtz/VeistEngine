#include "pch.h"

#include "PBRMaterial.h"

namespace Veist
{

PBRMaterial::PBRMaterial(const char* material_name, const VertexDescription& vertex_desc, 
	std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> occ_rgh_met, std::shared_ptr<Texture> emmissive) :
	m_albedo(albedo), 
	m_normal(normal), 
	m_occlusion_roughness_metallic(occ_rgh_met),
	m_emmissive(emmissive)
{

	createPipeline(material_name, vertex_desc);
	setUpDescriptorSet();

}


void PBRMaterial::setUpDescriptorSet()
{

	m_descriptor_set.setDescriptorSetLayout(MATERIAL_DESCRIPTOR_SET_NUMBER, m_pipeline.get());

	
	m_descriptor_set.bindCombinedSamplerImage(PBRTextures::Albedo, m_albedo.get(), { SamplerType::RepeatLinear });
	m_descriptor_set.bindCombinedSamplerImage(PBRTextures::Normal, m_normal.get(), { SamplerType::RepeatLinear });
	m_descriptor_set.bindCombinedSamplerImage(PBRTextures::OcclusionRoughnessMetallic, m_occlusion_roughness_metallic.get(), { SamplerType::RepeatLinear });
	m_descriptor_set.bindCombinedSamplerImage(PBRTextures::Emmissive, m_emmissive.get(), { SamplerType::RepeatLinear });
	

	m_descriptor_set.buildDescriptorSet();
}


void PBRMaterial::createPipeline(const char* material_name, const VertexDescription& vertex_desc) {

	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc);

}

}