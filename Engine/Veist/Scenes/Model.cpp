#include "pch.h"
#include "Model.h"


#include "Veist/Graphics/Vulkan/Buffers/VertexBuffer.h"

#include "Veist/Util/AssetLoader.h"

namespace Veist
{

Model::Model(std::string folder_filepath, std::string gltf_file)
{
	std::string gltf_filepath_str = folder_filepath;
	const char* gltf_filepath = gltf_filepath_str.append(gltf_file).c_str();

	m_mesh = AssetLoader::loadMeshFromGLTF(gltf_filepath);

	m_material = AssetLoader::loadPBRMaterialFromGLTF( gltf_filepath, folder_filepath );

}

}