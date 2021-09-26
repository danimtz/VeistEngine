#pragma once

//Will be rewritten later on into ECS/component system

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"



#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>



struct MeshData {

	std::vector<Vertex> vbuffer_data; //Vertex buffer data. 
	std::vector<uint16_t> index_data; //Assumes 16 bit index buffers (change from uint16_t to uint32_t for 32)
	uint32_t vbuffer_size;//in bytes
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
	Mesh(MeshData& data);

	std::shared_ptr<VertexBuffer> getVertexBuffer() const { return m_vertex_buffer; };
	std::shared_ptr<IndexBuffer> getIndexBuffer() const { return m_index_buffer; };

private:

	std::shared_ptr<VertexBuffer>	m_vertex_buffer;
	std::shared_ptr<IndexBuffer>	m_index_buffer;
};

