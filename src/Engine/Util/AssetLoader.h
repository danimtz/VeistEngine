#pragma once

#include "Engine/Logger.h"

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Mesh/Mesh.h"

#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Renderer/Vulkan/Images/ImageProperties.h"

class PBRMaterial;
class SkyboxMaterial;
//TODO: be reworked alongisde resource/asset system that stores loaded assets in a map etc
namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf file


	//semi-temporary rework into resource/asset system later 
	std::shared_ptr<Mesh> loadMeshFromGLTF(const char* gltf_filepath);

	std::shared_ptr<Texture> loadTextureFromFile(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB });
	std::shared_ptr<Cubemap> loadCubemapFromFiles(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz, const char* negz, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB });
	std::shared_ptr<Cubemap> loadCubemapFromHDRMap(const char* filepath, ImageFormat format = { VK_FORMAT_R16G16B16A16_SFLOAT});


	//std::shared_ptr<Texture> loadTextureFromGLTF(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB }); //TODO:
	std::shared_ptr<PBRMaterial> loadPBRMaterialFromGLTF(const char* material_name, const char* gltf_filepath, std::string folder_path, const VertexDescription& vertex_desc); //TODO: dont like the VertexDescription argument dependency
	

	std::shared_ptr<SkyboxMaterial> loadSkyboxMaterialFromCubemap(const char* material_name, const std::string& filepath, const VertexDescription& vertex_desc); //TODO: dont like the VertexDescription argument dependency
	std::shared_ptr<SkyboxMaterial> loadSkyboxMaterialFromHDRMap(const char* material_name, const std::string& filepath, const VertexDescription& vertex_desc); //TODO: dont like the VertexDescription argument dependency


}

