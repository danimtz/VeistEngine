#pragma once

#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#include "Engine/Util/AssetLoader.h"

//TODO
class Material
{
public:

	Material(const char* material_name,  const VertexDescription& vertex_desc);

	const DescriptorSet& descriptorSet() const { return m_descriptor_set; };
	const std::vector<std::shared_ptr<Texture>>& textures() const { return m_textures; };

	std::shared_ptr<GraphicsPipeline> pipeline() const { return m_pipeline; };

private:

	std::string m_name;
	std::shared_ptr<GraphicsPipeline> m_pipeline;
	std::vector< std::shared_ptr<Texture> > m_textures;
	DescriptorSet m_descriptor_set;

};

