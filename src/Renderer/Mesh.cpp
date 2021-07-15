#include "Mesh.h"


Mesh::Mesh() 
{

	m_vertices.resize(3);

	m_vertices[0].position = { 1.f, 1.f, 0.0f };
	m_vertices[1].position = { -1.f, 1.f, 0.0f };
	m_vertices[2].position = { 0.f,-1.f, 0.0f };

	//vertex colors, all green
	m_vertices[0].color = { 0.f, 1.f, 0.0f }; //pure green
	m_vertices[1].color = { 0.f, 1.f, 0.0f }; //pure green
	m_vertices[2].color = { 0.f, 1.f, 0.0f }; //pure green

	//ignore normals for now

	m_vertex_buffer = VertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
}