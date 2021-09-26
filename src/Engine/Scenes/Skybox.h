#pragma once

#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Material/SkyboxMaterial.h"
#include "Engine/Mesh/Mesh.h"


class Skybox
{
public:

	//filepath must be path to filename without extension. asset loader adds the _negx.jpg etc etc. TODO: rework this at some point
	Skybox(std::string material_name, std::string file_path);


	std::shared_ptr<Mesh>	mesh() const { return m_skybox_mesh; };
	std::shared_ptr<SkyboxMaterial> material() const { return m_material; };

private:

	std::shared_ptr<Mesh> m_skybox_mesh;
	std::shared_ptr<SkyboxMaterial> m_material;

};

