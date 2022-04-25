#pragma once

#include "Veist/Mesh/Mesh.h"
#include "Veist/Material/Material.h"
#include "Veist/Scenes/Model.h"



namespace Veist
{
/*
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

	};*/


	class CommandBuffer;

	struct MeshComponent
	{

		MeshComponent() = default;
		MeshComponent(std::vector<SubMesh> sub_meshes) : m_sub_meshes(sub_meshes) {}
		MeshComponent(const MeshComponent&) = default;
		

		void renderMesh(CommandBuffer& cmd, const std::vector<DescriptorSet>& descriptor_sets) //TODO maybe dont reference descriptor sets directly here call it PassInfo or whatever
		{
			for (auto& sub_mesh : m_sub_meshes)
			{
				sub_mesh.renderSubMesh(cmd, descriptor_sets);
			}
		};
		
		

		//TEMP
	//	Mesh* mesh() { return m_sub_meshes[0].m_mesh.get(); };
	//	Material* material() { return m_sub_meshes[0].m_material.get(); };

	private:

		std::vector<SubMesh> m_sub_meshes;
		//std::vector< std::shared_ptr<Material> > m_materials; //Copying material gonna be angery maybe due to unique pointer
		
		//ResourcePtr<Mesh> //TODO
		//ResourcePtr<Material>

	};


}
