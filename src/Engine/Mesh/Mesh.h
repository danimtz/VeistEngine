#pragma once

//Will be rewritten later on into ECS/component system

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"



#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "Engine/Util/AssetLoader.h"

struct Vertex //leftover. remove later
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	//uv
	//tangent
};

class VertexBuffer;
class IndexBuffer;

class Mesh
{
public:
	
	Mesh();
	Mesh(const char* mesh_filepath);


	std::shared_ptr<VertexBuffer> getVertexBuffer() const { return m_vertex_buffer; };
	std::shared_ptr<IndexBuffer> getIndexBuffer() const { return m_index_buffer; };

private:

	std::vector<Vertex>				m_vertices;
	std::shared_ptr<VertexBuffer>	m_vertex_buffer;
	std::shared_ptr<IndexBuffer>	m_index_buffer;
};

