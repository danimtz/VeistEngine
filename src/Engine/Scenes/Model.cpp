#include "Model.h"


#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"

Model::Model(const char* mesh_filepath, const char* material_name, const char* material_filepath)
{

	
	m_mesh = std::make_shared<Mesh>(mesh_filepath);
	m_material = std::make_shared<Material>(material_name, material_filepath, m_mesh->getVertexBuffer()->getInputDescription());

}

