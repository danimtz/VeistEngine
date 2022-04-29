#include "pch.h"
#include "EngineResources.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"

namespace Veist
{

	

	//TODO all these arrays should just be a "Material settings" struct
	/*static constexpr uint32_t material_descriptor_set_number[] = {
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

	static constexpr DepthTest material_depth_settings[] = {
		DepthTest::ReadWrite,
		DepthTest::ReadWrite,
		DepthTest::ReadWrite,
		DepthTest::Read
	};
	*/

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



	static void declare_material_settings(std::vector<MaterialSettings>& material_settings)
	{

		MaterialSettings PBRForward = {};
		PBRForward.vertex_description = getVertexDescription(EngineResources::MaterialTypes::PBRMaterial); //This is for vertex description generation
		PBRForward.descriptor_set_number = 1;
		PBRForward.attachment_count = 1;
		PBRForward.vertex_shader_name = "PBRForward.vert";
		PBRForward.fragment_shader_name = "PBRForward.frag";
		PBRForward.depth_setting = DepthTest::ReadWrite;

		MaterialSettings Skybox = {};
		Skybox.vertex_description = getVertexDescription(EngineResources::MaterialTypes::SkyboxMaterial); //This is for vertex description generation
		Skybox.descriptor_set_number = 0;
		Skybox.attachment_count = 1;
		Skybox.vertex_shader_name = "Skybox.vert";
		Skybox.fragment_shader_name = "Skybox.frag";
		Skybox.depth_setting = DepthTest::ReadWrite;

		MaterialSettings GBuffer = {};
		GBuffer.vertex_description = getVertexDescription(EngineResources::MaterialTypes::DeferredGBufferMaterial); //This is for vertex description generation
		GBuffer.descriptor_set_number = 2;
		GBuffer.attachment_count = 3;
		GBuffer.vertex_shader_name = "GBuffer.vert";
		GBuffer.fragment_shader_name = "GBuffer.frag";
		GBuffer.depth_setting = DepthTest::ReadWrite;

		MaterialSettings DeferredLighting = {};
		DeferredLighting.vertex_description = getVertexDescription(EngineResources::MaterialTypes::DeferredLightingMaterial); //This is for vertex description generation
		DeferredLighting.descriptor_set_number = 0;
		DeferredLighting.attachment_count = 1;
		DeferredLighting.vertex_shader_name = "DeferredLighting.vert";
		DeferredLighting.fragment_shader_name = "DeferredLighting.frag";
		DeferredLighting.depth_setting = DepthTest::Read;


		MaterialSettings EditorBillboard = {};
		EditorBillboard.vertex_description = getVertexDescription(EngineResources::MaterialTypes::EditorBillboard); //This is for vertex description generation
		EditorBillboard.descriptor_set_number = 1;
		EditorBillboard.attachment_count = 1;
		EditorBillboard.vertex_shader_name = "CameraViewBillboard.vert";
		EditorBillboard.fragment_shader_name = "CameraViewBillboard.frag";
		EditorBillboard.depth_setting = DepthTest::None;


		material_settings.emplace_back(PBRForward);
		material_settings.emplace_back(Skybox);
		material_settings.emplace_back(GBuffer);
		material_settings.emplace_back(DeferredLighting);

	}


	EngineResources::EngineResources()
	{

		declare_material_settings(m_material_settings);

		m_material_types = std::make_unique< std::vector<MaterialType> >();

		for (auto& settings : m_material_settings)
		{
			m_material_types.get()->emplace_back(settings);
		}


	}


	MaterialType* EngineResources::getMaterialType(MaterialTypes type)
	{
		return &m_material_types->at(type);
	}

}