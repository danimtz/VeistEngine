#pragma once

#include "Veist/Scenes/Skybox.h"

namespace Veist
{

struct SkyboxComponent
{
	SkyboxComponent() = default;
	SkyboxComponent(const SkyboxComponent&) = default;
	

	SkyboxComponent(Skybox& skybox) : m_skybox_mesh(skybox.mesh()), m_material(skybox.material()) {}; //TEMPORARY, TODO: remove this function (when asset system is implemented etc)
	
	Mesh* mesh() { return m_skybox_mesh.get(); };
	SkyboxMaterial* material() { return m_material.get(); };

	
private:

	std::shared_ptr<Mesh> m_skybox_mesh;
	std::shared_ptr<SkyboxMaterial> m_material;

};

}
