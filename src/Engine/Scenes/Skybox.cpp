
#include "Skybox.h"

#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"


static void fillSkyboxMeshData(MeshData& mesh_data)
{
	
	mesh_data.vbuffer_size = 8*sizeof(Vertex);
	std::vector<Vertex>& vertices = mesh_data.vbuffer_data;
	vertices.resize(8);

	vertices[0].position = glm::vec3{-1, -1, -1};
	vertices[1].position = glm::vec3{ 1, -1, -1};
	vertices[2].position = glm::vec3{1, 1, -1};
	vertices[3].position = glm::vec3{-1, 1, -1};
	vertices[4].position = glm::vec3{-1, -1, 1};
	vertices[5].position = glm::vec3{1, -1, 1};
	vertices[6].position = glm::vec3{1, 1, 1};
	vertices[7].position = glm::vec3{-1, 1, 1};

	mesh_data.index_count = 36;
	uint16_t indices[36] = {0, 1, 3, 3, 1, 2, 1, 5, 2, 2, 5, 6, 5, 4, 6, 6, 4, 7, 4, 0, 7, 7, 0, 3, 3, 2, 7, 7, 2, 6, 4, 5, 0, 0, 5, 1};

	mesh_data.index_data.resize(mesh_data.index_count);
	memcpy(mesh_data.index_data.data(), indices, mesh_data.index_count * mesh_data.index_size);

	
	VertexDescription vertex_desc = {0,
		{{VertexAttributeType::Float3, "POSITION"},
		{VertexAttributeType::Float3, "NAN"},
		{VertexAttributeType::Float4, "NAN"},
		{VertexAttributeType::Float2, "NAN"}}
	};
	
	mesh_data.description = vertex_desc;

}

Skybox::Skybox(std::string material_name, std::string file_path)
{

	MeshData skybox_data;
	fillSkyboxMeshData(skybox_data);

	m_skybox_mesh = std::make_shared<Mesh>(skybox_data); //Vertices not working

	m_material = AssetLoader::loadSkyboxMaterialFromFilepath(material_name.c_str(), file_path, m_skybox_mesh->getVertexBuffer()->getInputDescription());

}