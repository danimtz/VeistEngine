#include "Material.h"

Material::Material(const char* material_name, const char* shader_filepath, const VertexDescription& vertex_desc)
{
	m_name = material_name;
	m_pipeline = GraphicsPipeline::Create(material_name, shader_filepath, vertex_desc); //

}