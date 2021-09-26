#pragma once

#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/Vulkan/Images/Image.h"
#include "Engine/Renderer/Vulkan/Descriptors/DescriptorSet.h"

#include "Engine/Util/AssetLoader.h"


class Material
{
public:

	Material() = default;
	virtual ~Material() = default;

	const DescriptorSet& descriptorSet() const { return m_descriptor_set; };
	const std::shared_ptr<GraphicsPipeline> pipeline() const { return m_pipeline; };

	

protected:
	
	//Configures the descriptor set for the material should be called from constructor of subclass
	virtual void setUpDescriptorSet() = 0;
	virtual void createPipeline(const char* material_name, const VertexDescription& vertex_desc) = 0;

	std::shared_ptr<GraphicsPipeline> m_pipeline;
	DescriptorSet m_descriptor_set;

	static const uint32_t MATERIAL_DESCRIPTOR_SET_NUMBER = 1;

};
