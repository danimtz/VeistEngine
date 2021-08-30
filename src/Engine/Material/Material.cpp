#include "Material.h"




Material::Material(MaterialData& data) : m_data(data)
{
	
	m_descriptor_set.setDescriptorSetLayout(MATERIAL_DESCRIPTOR_SET_NUMBER, m_data.pipeline().get());

	for (int i = 0; i < m_data.textures().size(); i++) {
		m_descriptor_set.bindCombinedSamplerTexture(i, m_data.textures()[i].get());
	}
	
	
	m_descriptor_set.buildDescriptorSet();
}



MaterialData::MaterialData(const char* material_name, const VertexDescription& vertex_desc) {

	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc); 
	
}

void MaterialData::addTexture(std::shared_ptr<Texture> texture, PBRTextures tex_type)
{
	if (m_textures.size() >= tex_type) {
		m_textures.resize(tex_type+1);
	}
	m_textures[tex_type] = texture;
}
