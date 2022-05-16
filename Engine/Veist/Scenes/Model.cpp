#include "pch.h"
#include "Model.h"


#include "Veist/Graphics/Vulkan/Buffers/VertexBuffer.h"

#include "Veist/Util/AssetLoader.h"

#include "Veist/Graphics/RenderModule.h"

namespace Veist
{


	void SubMesh::renderSubMesh(CommandBuffer& cmd, const std::vector<DescriptorSet>& descriptor_sets, const uint32_t mesh_id, const bool bind_material)
	{
		if (bind_material)
		{
			cmd.bindMaterial(*m_material);
		}
		

		for (auto& descriptor_set : descriptor_sets)
		{
			cmd.bindDescriptorSet(descriptor_set);
		}

		cmd.bindVertexBuffer(*m_mesh->getVertexBuffer());
		cmd.bindIndexBuffer(*m_mesh->getIndexBuffer());

		cmd.drawIndexed(m_mesh->getIndexBuffer()->getIndexCount(), mesh_id);

	}




	Model::Model(std::string folder_filepath, std::string gltf_file)
	{
		std::string gltf_filepath_str = folder_filepath;
		const char* gltf_filepath = gltf_filepath_str.append(gltf_file).c_str();

		m_mesh = AssetLoader::loadMeshFromGLTF(gltf_filepath);

		m_material = AssetLoader::loadPBRMaterialFromGLTF( gltf_filepath, folder_filepath );

	}

}