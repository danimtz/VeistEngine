#pragma once

#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Mesh/Mesh.h"
#include "Veist/Material/Material.h"


namespace Veist
{

class Skybox
{
public:

	static std::unique_ptr<Skybox> createFromCubemap(const std::string& cubemap_files_path);
	static std::unique_ptr<Skybox> createFromEquirectMap(const std::string& env_map);

	//filepath must be path to filename without extension. asset loader adds the _negx.jpg etc etc. TODO: rework this at some point
	
	Skybox(const std::string& file_path, bool isCubemap);
	Skybox(const Skybox& other) 
	{ 
		m_skybox_mesh = other.m_skybox_mesh; 
		m_material = other.m_material; 
	};

	std::shared_ptr<Mesh>	mesh() const { return m_skybox_mesh; };
	std::shared_ptr<Material> material() const { return m_material; };

private:

	std::shared_ptr<Mesh> m_skybox_mesh;
	std::shared_ptr<Material> m_material;

};

}