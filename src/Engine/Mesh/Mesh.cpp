#include "Mesh.h"

#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"
#include "Engine/Renderer/Vulkan/Buffers/IndexBuffer.h"

Mesh::Mesh() 
{
}

Mesh::Mesh(MeshData& data)
{

	//create vertex buffers
	m_vertex_buffer = std::make_shared<VertexBuffer>(data.vbuffer_data.data(), data.vbuffer_size); //index buffer data is mixed with vertex, its occupying extra space atm but thats the only evil
	m_vertex_buffer->setInputDescription(data.description);

	//Create index buffers
	m_index_buffer = std::make_shared<IndexBuffer>(data.index_data.data(), data.index_count, data.index_size);

}