#include "Model.h"


#include "Engine/Renderer/Vulkan/Buffers/VertexBuffer.h"

Model::Model(const char* mesh_filepath, const char* material_name)
{

	
	m_mesh = AssetLoader::loadMeshFromGLTF(mesh_filepath);


	//TODO: Dont have resources take in filepaths in constructor. rework this
	m_material = std::make_shared<Material>(material_name, m_mesh->getVertexBuffer()->getInputDescription());

}

