#include "pch.h"
#include "Model.h"


#include "Veist/Graphics/Vulkan/Buffers/VertexBuffer.h"

namespace Veist
{

Model::Model(std::string folder_filepath, std::string material_name, std::string gltf_file)
{
	std::string gltf_filepath_str = folder_filepath;
	const char* gltf_filepath = gltf_filepath_str.append(gltf_file).c_str();

	m_mesh = AssetLoader::loadMeshFromGLTF(gltf_filepath);

	m_material = AssetLoader::loadPBRMaterialFromGLTF(material_name.c_str(), gltf_filepath, folder_filepath, m_mesh->getVertexBuffer()->getInputDescription());

}

}