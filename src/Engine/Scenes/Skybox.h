#pragma once

#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Material/SkyboxMaterial.h"
#include "Engine/Mesh/Mesh.h"


class Skybox
{
public:

	static std::unique_ptr<Skybox> createFromCubemap(const std::string& material_name, const std::string& cubemap_files_path);
	static std::unique_ptr<Skybox> createFromEquirectMap(const std::string& material_name, const std::string& env_map);

	//filepath must be path to filename without extension. asset loader adds the _negx.jpg etc etc. TODO: rework this at some point
	
	Skybox(const std::string& material_name, const std::string& file_path, bool isCubemap);
	

	std::shared_ptr<Mesh>	mesh() const { return m_skybox_mesh; };
	std::shared_ptr<SkyboxMaterial> material() const { return m_material; };

private:

	std::shared_ptr<Mesh> m_skybox_mesh;
	std::shared_ptr<SkyboxMaterial> m_material;

};

