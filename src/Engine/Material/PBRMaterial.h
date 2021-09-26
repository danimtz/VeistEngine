#pragma once

#include "Material.h"




class PBRMaterial : public Material
{
public:
	
	PBRMaterial(const char* material_name, const VertexDescription& vertex_desc,
		std::shared_ptr<Texture> albedo, std::shared_ptr<Texture> normal, std::shared_ptr<Texture> occ_rgh_met, std::shared_ptr<Texture> emmissive);
	

private:

	void setUpDescriptorSet() override;
	void createPipeline(const char* material_name, const VertexDescription& vertex_desc) override;


	std::shared_ptr<Texture> m_albedo;
	std::shared_ptr<Texture> m_normal;
	std::shared_ptr<Texture> m_occlusion_roughness_metallic;
	std::shared_ptr<Texture> m_emmissive;

	enum PBRTextures : uint32_t
	{
		Albedo = 0,
		Normal = 1,
		OcclusionRoughnessMetallic = 2,
		Emmissive = 3
	};
};

