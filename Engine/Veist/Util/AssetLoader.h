#pragma once

#include "Logger.h"

#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"
#include "Veist/Mesh/Mesh.h"

#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Images/ImageProperties.h"

namespace Veist
{

class Material;
class SkyboxMaterial;
class MeshComponent;
//TODO: be reworked alongisde resource/asset system that stores loaded assets in a map etc
namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf files


	//semi-temporary rework into resource/asset system later 
	//std::shared_ptr<Model> loadModelFromGLTF(const char* gltf_filepath);

	std::shared_ptr<Mesh> loadMeshFromGLTF(const char* gltf_filepath);

	//TODO add support for reading textures directly from GLB file through tinygltf
	std::shared_ptr<Texture> loadTextureFromFile(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB }, bool mipmaps = true);
	std::shared_ptr<Cubemap> loadCubemapFromFiles(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB });
	std::shared_ptr<Cubemap> loadCubemapFromEquirectMap(const char* filepath, bool mipmaps = false);


	//std::shared_ptr<Texture> loadTextureFromGLTF(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB }); //TODO:
	std::shared_ptr<Material> loadPBRMaterialFromGLTF(const char* gltf_filepath, std::string folder_path); 
	

	std::shared_ptr<Material> loadSkyboxMaterialFromCubemap(const std::string& filepath);
	std::shared_ptr<Material> loadSkyboxMaterialFromEquirectMap(const std::string& filepath);
	


	MeshComponent loadMeshComponent(const char* gltf_filepath);

}

}
