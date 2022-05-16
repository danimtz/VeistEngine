#pragma once

#include "Veist/Mesh/Mesh.h"
#include "Veist/Material/Material.h"


//TODO: Rename this file to SubMesh and remove Model

namespace Veist
{

	class CommandBuffer;
	class SubMesh
	{
	public:
		
		SubMesh() = default;
		SubMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : m_mesh(mesh), m_material(material) {};


		void renderSubMesh(CommandBuffer& cmd, const std::vector<DescriptorSet>& descriptor_sets, const uint32_t mesh_id, const bool bind_material);

	//private://TEMP privatize later
		
		std::shared_ptr<Mesh>	m_mesh;
		std::shared_ptr<Material> m_material;

	};



	class Model
	{
	public:

		Model(std::string folder_filepath, std::string gltf_file);


		std::shared_ptr<Mesh>	mesh() const { return m_mesh; };
		std::shared_ptr<Material> material() const { return m_material; };


		//temp
		glm::mat4& modelMatrix() const { return m_model_mat; };

	private:

		std::shared_ptr<Mesh>	m_mesh;
		std::shared_ptr<Material> m_material;

		//temp until ECS
		mutable glm::mat4 m_model_mat{1.0f};

	};

}