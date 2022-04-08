#pragma once

#include "Veist/Mesh/Mesh.h"
#include "Veist/Material/Material.h"
#include "Veist/Scenes/Model.h"

namespace Veist
{

	struct MeshComponent
	{
	
		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;
		MeshComponent(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : m_mesh(mesh), m_material(material) {};


		MeshComponent(Model& model) : m_mesh(model.mesh()), m_material(model.material()) {}; //TEMPORARY, TODO: remove this function when model class is deprecated (when asset system is implemented etc)

		Mesh* mesh() { return m_mesh.get(); };
		Material* material() { return m_material.get(); };

	private:


		std::shared_ptr<Mesh>	m_mesh;
		std::shared_ptr<Material> m_material;
		//ResourcePtr<Mesh> //TODO
		//ResourcePtr<Material>

	};


}
