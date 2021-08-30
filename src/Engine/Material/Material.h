#pragma once

#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#include "Engine/Util/AssetLoader.h"

//TODO


class MaterialData
{
public:
	
	enum PBRTextures : uint32_t
	{
		Albedo = 0,
		Normal = 1,
		OcclusionRoughnessMetallic = 2,
		Emmissive = 3
	};
	
	MaterialData() = default;
	MaterialData(const char* material_name, const VertexDescription& vertex_desc);

	void addTexture(std::shared_ptr<Texture> texture, PBRTextures tex_type);

	const std::vector<std::shared_ptr<Texture>>& textures() const { return m_textures; };
	std::shared_ptr<GraphicsPipeline> pipeline() const { return m_pipeline; };

private:

	std::shared_ptr<GraphicsPipeline> m_pipeline;
	std::vector< std::shared_ptr<Texture> > m_textures;

};



class Material
{
public:

	Material(MaterialData& data);

	const DescriptorSet& descriptorSet() const { return m_descriptor_set; };
	const MaterialData& materialData() const { return m_data; };


private:

	MaterialData m_data;
	
	DescriptorSet m_descriptor_set;

	static const uint32_t MATERIAL_DESCRIPTOR_SET_NUMBER = 1;

};

