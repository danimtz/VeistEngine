#pragma once

#include "Engine/Logger.h"

#include "Engine/Renderer/Vulkan/Buffers/VertexDescription.h"
#include "Engine/Mesh/Mesh.h"

//WIll be reworked alongisde resource/asset system
namespace AssetLoader {


	//This will need to be expanded upon. for now just load a mesh from a gltf file


	//semi-temporary rework into resource/asset system later
	std::shared_ptr<Mesh> meshFromGLTF(const char* filepath);



}

