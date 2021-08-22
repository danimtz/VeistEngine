#pragma once

#include "Engine/Logger.h"

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Mesh/Mesh.h"

#include "Engine/Renderer/Vulkan/Images/Image.h"

//TODO: be reworked alongisde resource/asset system that stores loaded assets in a map etc
namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf file


	//semi-temporary rework into resource/asset system later 
	std::shared_ptr<Mesh> loadMeshFromGLTF(const char* filepath);

	std::shared_ptr<Texture> loadTextureFromFile(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB });

	//std::shared_ptr<Texture> loadTextureFromGLTF(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB }); //TODO:
	//std::shared_ptr<Material> loadMaterialFromGLTF(const char* filepath, ImageFormat format = { VK_FORMAT_R8G8B8A8_SRGB }); //TODO:


}

