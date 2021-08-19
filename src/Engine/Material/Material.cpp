#include "Material.h"

Material::Material(const char* material_name,  const VertexDescription& vertex_desc)
{
	m_name = material_name;
	m_pipeline = std::make_shared<GraphicsPipeline>(material_name, vertex_desc); //

}