#include "pch.h"
#include "EngineResources.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"

namespace Veist
{


	static constexpr uint32_t material_types_count = uint32_t(EngineResources::MaterialTypes::MaxMaterialTypes);
	
    static constexpr uint32_t material_descriptor_index[] = {
        1,//PBRMaterial
        0,//SKyboxMaterial  
    };

	static constexpr const char* material_type_names[] = {
		"PBRforward",
		"Skybox",
	};

	static VertexDescription getDefaultVertexDescription()
	{
		return VertexDescription(0, {{VertexAttributeType::Float3, "position"},
									{VertexAttributeType::Float3, "normal"},
									{VertexAttributeType::Float4, "tangent"},
									{VertexAttributeType::Float2, "ub"}});
	};


	EngineResources::EngineResources()
	{
		m_material_types = std::make_unique< std::vector<MaterialType> >();

		for (uint32_t i = 0; i < material_types_count; i++)
		{
			m_material_types.get()->emplace_back(material_type_names[i], getDefaultVertexDescription(), material_descriptor_index[i]);
		}


	}


	MaterialType* EngineResources::getMaterialType(MaterialTypes type)
	{
		return &m_material_types->at(type);
	}

}