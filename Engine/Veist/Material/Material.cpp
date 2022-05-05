#include "pch.h"

#include "Material.h"

namespace Veist
{


	Material::Material(MaterialType* type, MaterialData data) : m_material_type(type), m_material_data(data)
	{
	/*
		m_descriptor_set.setDescriptorSetLayout( type->descSetNumber(), type->pipelineBuilder());

		uint32_t count = 0;
		for (auto& image : data.textures())
		{
			m_descriptor_set.bindCombinedSamplerImage(count, image.get(), { SamplerType::RepeatLinear });
			count++;
		}

		m_descriptor_set.buildDescriptorSet();
*/
		//DescriptorSet(uint32_t set_number, std::vector<Descriptor> & descriptor_bindings);
		
		std::vector<Descriptor> bindings;
		for (auto& image : data.textures())
		{
			bindings.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, image.get(), SamplerType::RepeatLinear);
		}

		m_descriptor_set = std::make_unique<DescriptorSet>(type->descSetNumber(), bindings);
	}
	

}