#pragma once

//Will be rewritten later on into ECS/component system

#include "Renderer/Buffer.h"
#include <vector>
#include <memory>
#include <string>

#include <glm/glm.hpp>


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
	Mesh(std::string mesh_filepath){};

	void loadMeshFromFile(std::string mesh_filepath){};

	std::shared_ptr<VertexBuffer> getVertexBuffer() { return m_vertex_buffer; };

private:

	std::vector<Vertex>				m_vertices;
	std::shared_ptr<VertexBuffer>	m_vertex_buffer;

};

