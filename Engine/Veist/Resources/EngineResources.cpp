#include "pch.h"
#include "EngineResources.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"

namespace Veist
{


	static constexpr uint32_t material_types_count = uint32_t(EngineResources::MaterialTypes::MaxMaterialTypes);
	
    static constexpr uint32_t material_descriptor_set_number[] = {
        1,//PBRMaterial
        0,//SKyboxMaterial
		1,//GBuffer
		0,//DeferredLighting
    };

	//TODO: myabe attachment count should be reflected from shaders
	static constexpr uint32_t material_attachment_count[] = {
		1,//PBRMaterial
		1,//SKyboxMaterial
		4,//GBuffer
		1,//DeferredLighting
	};

	static constexpr const char* material_type_names[] = {
		"PBRforward",
		"Skybox",
		"GBuffer",
		"DeferredLighting"
	};

	static VertexDescription getVertexDescription(EngineResources::MaterialTypes type)
	{
		switch (type) 
		{
			case EngineResources::MaterialTypes::DeferredLightingMaterial: //Quad
				return VertexDescription();

			default:
				return VertexDescription(0, { {VertexAttributeType::Float3, "position"},
										{VertexAttributeType::Float3, "normal"},
										{VertexAttributeType::Float4, "tangent"},
										{VertexAttributeType::Float2, "uv"} });

		}
		
		
	};


	EngineResources::EngineResources()
	{
		m_material_types = std::make_unique< std::vector<MaterialType> >();

		for (uint32_t i = 0; i < material_types_count; i++)
		{
			m_material_types.get()->emplace_back(material_type_names[i], getVertexDescription(EngineResources::MaterialTypes(i)), material_attachment_count[i], material_descriptor_set_number[i]);
		}


	}


	MaterialType* EngineResources::getMaterialType(MaterialTypes type)
	{
		return &m_material_types->at(type);
	}

}