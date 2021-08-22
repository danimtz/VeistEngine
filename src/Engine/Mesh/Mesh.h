#pragma once

//Will be rewritten later on into ECS/component system

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"



#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>



struct MeshData {

	std::vector<unsigned char> vbuffer_data; //Vertex buffer data. 
	std::vector<unsigned char> index_data;
	uint32_t index_count;
	uint32_t index_size = { 2 };
	VertexDescription description;

};

class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
	
	Mesh();
	Mesh(const char* mesh_filepath);
	Mesh(MeshData& data);

	std::shared_ptr<VertexBuffer> getVertexBuffer() const { return m_vertex_buffer; };
	std::shared_ptr<IndexBuffer> getIndexBuffer() const { return m_index_buffer; };

private:

	std::vector<Vertex>				m_vertices;
	std::shared_ptr<VertexBuffer>	m_vertex_buffer;
	std::shared_ptr<IndexBuffer>	m_index_buffer;
};

