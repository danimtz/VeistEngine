#pragma once

#include "Veist/Material/Material.h"


namespace Veist
{

class SkyboxMaterial : public Material
{
public:

	SkyboxMaterial(const char* material_name, const VertexDescription& vertex_desc, std::shared_ptr<Cubemap> cubemap);

private:

	void setUpDescriptorSet() override;
	void createPipeline(const char* material_name, const VertexDescription& vertex_desc) override;

	std::shared_ptr<Cubemap> m_cubemap;

	static const uint32_t SKYBOX_MATERIAL_DESCRIPTOR_SET_NUMBER = 0;

};


}