#include "pch.h"

#include "Material.h"

namespace Veist
{


	Material::Material(MaterialType* type, MaterialData data) : m_material_type(type), m_material_data(data)
	{

		m_descriptor_set.setDescriptorSetLayout( type->descSetNumber(), type->pipelineBuilder());

		uint32_t count = 0;
		for (auto& image : data.textures())
		{
			m_descriptor_set.bindCombinedSamplerImage(count, image.get(), { SamplerType::RepeatLinear });
			count++;
		}

		m_descriptor_set.buildDescriptorSet();

	}
	

}