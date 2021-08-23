#include "Material.h"




Material::Material(const char* material_name,  const VertexDescription& vertex_desc)
{
	m_name = material_name;
	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc); //


	//EXTREMELY TEMPORARY AS IT ALWAYS LOADS THE SAME TEXTURE. THIS SHOULD LOAD teXTURES AUTOMATICALLYvFROM A GLTF FILE OR SOMETHING
	m_textures.push_back(AssetLoader::loadTextureFromFile("..\\..\\assets\\DamagedHelmet\\Default_albedo.jpg"));

	m_descriptor_set.setDescriptorSetLayout(MATERIAL_DESCRIPTOR_SET_NUMBER, m_pipeline.get());

	for (int i = 0; i < m_textures.size(); i++) {
		m_descriptor_set.bindCombinedSamplerTexture(i, m_textures[i].get());
	}
	
	
	m_descriptor_set.buildDescriptorSet();
}