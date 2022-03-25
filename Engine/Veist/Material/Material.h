#pragma once

//#include "Veist/Graphics/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
//#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Descriptors/DescriptorSet.h"

#include "Veist/Util/AssetLoader.h"

#include "Veist/Material/MaterialType.h"
#include "Veist/Material/MaterialData.h"

namespace Veist
{

	class Material
	{
	public:
		
		Material(MaterialType* type, MaterialData data);
		~Material() = default;

		const DescriptorSet& descriptorSet() const { return m_descriptor_set; };

		MaterialType* materialType() const { return m_material_type; };

	private:

		MaterialType* m_material_type;

		MaterialData m_material_data;

		DescriptorSet m_descriptor_set;
	};


}