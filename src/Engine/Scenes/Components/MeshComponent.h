#pragma once

#include "Engine/Mesh/Mesh.h"
#include "Engine/Material/PBRMaterial.h"

struct MeshComponent
{
	
	MeshComponent() = default;
	MeshComponent(const MeshComponent&) = default;
	MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<PBRMaterial> material) : m_mesh(mesh), m_material(material) {};

	std::shared_ptr<Mesh>	m_mesh;
	std::shared_ptr<PBRMaterial> m_material;
	//ResourcePtr<Mesh> //TODO
	//ResourcePtr<Material>

};

struct TestComponent
{

	TestComponent() = default;
	TestComponent(const TestComponent&) = default;
	TestComponent(uint32_t val1, uint32_t val2) : m_value1(val1), m_value2(val2) {};

	uint32_t	m_value1;
	uint32_t	m_value2;


};

