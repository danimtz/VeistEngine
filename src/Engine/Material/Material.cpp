#include "Material.h"




Material::Material(const char* material_name,  const VertexDescription& vertex_desc)
{
	m_name = material_name;
	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc); //


	//EXTREMELY TEMPORARY AS IT ALWAYS LOADS THE SAME TEXTURE. THIS SHOULD LOAD teXTURES AUTOMATICALLYvFROM A GLTF FILE OR SOMETHING
	m_textures.push_back(AssetLoader::loadTextureFromFile("..\\..\\assets\\DamagedHelmet\\Default_albedo.jpg"));

	m_descriptor_set.setDescriptorSetLayout(2, m_pipeline.get());

	
	m_descriptor_set.buildDescriptorSet();
}