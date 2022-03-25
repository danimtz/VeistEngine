#include "pch.h"

#include "Skybox.h"

#include "Veist/Graphics/Vulkan/Buffers/VertexBuffer.h"


namespace Veist
{

static void fillSkyboxMeshData(MeshData& mesh_data)
{
	
	mesh_data.vbuffer_size = 8*sizeof(Vertex);
	std::vector<Vertex>& vertices = mesh_data.vbuffer_data;
	vertices.resize(8);

	vertices[0].position = glm::vec3{-0.5, -0.5, -0.5};
	vertices[1].position = glm::vec3{ 0.5, -0.5, -0.5};
	vertices[2].position = glm::vec3{0.5, 0.5, -0.5};
	vertices[3].position = glm::vec3{-0.5, 0.5, -0.5};
	vertices[4].position = glm::vec3{-0.5, -0.5, 0.5};
	vertices[5].position = glm::vec3{0.5, -0.5, 0.5};
	vertices[6].position = glm::vec3{0.5, 0.5, 0.5};
	vertices[7].position = glm::vec3{-0.5, 0.5, 0.5};

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

std::unique_ptr<Skybox> Skybox::createFromCubemap( const std::string& file_path)
{
	return std::make_unique<Skybox>( file_path, true);
}


std::unique_ptr<Skybox> Skybox::createFromEquirectMap( const std::string& file_path)
{
	return std::make_unique<Skybox>( file_path, false);
}




Skybox::Skybox(const std::string& cubemap_files_path, bool isCubemap)
{

	MeshData skybox_data;
	fillSkyboxMeshData(skybox_data);

	m_skybox_mesh = std::make_shared<Mesh>(skybox_data); //Vertices not working

	if (isCubemap)
	{
		m_material = AssetLoader::loadSkyboxMaterialFromCubemap( cubemap_files_path);
	}
	else
	{
		
		m_material = AssetLoader::loadSkyboxMaterialFromEquirectMap( cubemap_files_path);
		
		
	}
	
}

}