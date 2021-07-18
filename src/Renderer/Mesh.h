#pragma once

//Will be rewritten later on into ECS/component system

#include "Renderer/Buffer.h"
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>

#include "Util/AssetLoader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	//uv
	//tangent
};


class Mesh
{
public:
	
	Mesh();
	Mesh(const char* mesh_filepath);


	std::shared_ptr<VertexBuffer> getVertexBuffer() { return m_vertex_buffer; };
	std::shared_ptr<IndexBuffer> getIndexBuffer() { return m_index_buffer; };

private:

	std::vector<Vertex>				m_vertices;
	std::shared_ptr<VertexBuffer>	m_vertex_buffer;
	std::shared_ptr<IndexBuffer>	m_index_buffer;
};

