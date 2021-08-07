#include "Mesh.h"

#include "Engine/Renderer/Vulkan/Buffers/VulkanVertexBuffer.h"
#include "Engine/Renderer/Vulkan/Buffers/VulkanIndexBuffer.h"

Mesh::Mesh() 
{

	m_vertices.resize(3);

	m_vertices[0].position = { 0.f, -0.5f, 0.0f };
	m_vertices[1].position = { 0.5f, 0.5f, 0.0f };
	m_vertices[2].position = { -0.5f, 0.5f, 0.0f };

	//vertex colors, all green
	m_vertices[0].color = { 0.f, 1.f, 0.0f }; 
	m_vertices[1].color = { 0.f, 0.f, 1.0f }; 
	m_vertices[2].color = { 1.f, 0.f, 0.0f }; 

	//ignore normals for now

	m_vertex_buffer = std::make_shared<VertexBuffer>(m_vertices.data(), m_vertices.size() * sizeof(Vertex));


	VertexDescription vertex_desc;
	vertex_desc = { 0, {			//Must match vertex used iin meshes
		{VertexAttributeType::Float3, "position"},
		{VertexAttributeType::Float3, "normal"},
		{VertexAttributeType::Float3, "color"} } };



	m_vertex_buffer->setInputDescription(vertex_desc);
}


Mesh::Mesh(const char* mesh_filepath)
{


	AssetLoader::MeshInfo mesh_info;  
	AssetLoader::loadMeshFromGLTF(mesh_filepath, mesh_info);

	//create vertex buffers
	m_vertex_buffer = std::make_shared<VertexBuffer>(mesh_info.vbuffer_data.data(), mesh_info.vbuffer_data.size()); //index buffer data is mixed with vertex, its occupying extra space atm but thats the only evil
	m_vertex_buffer->setInputDescription(mesh_info.description);

	//Create index buffers
	m_index_buffer = std::make_shared<IndexBuffer>(mesh_info.index_data.data(), mesh_info.index_count, mesh_info.index_size);

}