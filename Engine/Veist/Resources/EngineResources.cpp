#include "pch.h"
#include "EngineResources.h"
#include "Veist/Graphics/Vulkan/Buffers/VertexDescription.h"

namespace Veist
{



	static VertexDescription getVertexDescription(EngineResources::MaterialTypes type)
	{
		switch (type)
		{
		case EngineResources::MaterialTypes::DeferredLightingMaterial: //Quad
			return VertexDescription();


		case EngineResources::MaterialTypes::BillboardMaterial:
			return VertexDescription(0, { {VertexAttributeType::Float3, "position"},
									{VertexAttributeType::Float3, "NaN"},
									{VertexAttributeType::Float4, "NaN"},
									{VertexAttributeType::Float2, "NaN"} });

		default:
			return VertexDescription(0, { {VertexAttributeType::Float3, "position"},
									{VertexAttributeType::Float3, "normal"},
									{VertexAttributeType::Float4, "tangent"},
									{VertexAttributeType::Float2, "uv"} });

		}


	};




	static void fillBillboardMeshData(MeshData& mesh_data)
	{

		mesh_data.description = getVertexDescription(EngineResources::MaterialTypes::BillboardMaterial);
		
		//TODO
		mesh_data.vbuffer_data = ;
		mesh_data.index_data = 
		
		mesh_data.vbuffer_size = mesh_data.vbuffer_data.size() * mesh_data.description.getStride();
		mesh_data.index_count = 6;
	}





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
		EditorBillboard.vertex_description = getVertexDescription(EngineResources::MaterialTypes::BillboardMaterial); //This is for vertex description generation
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
		//Material settings

		declare_material_settings(m_material_settings);

		m_material_types = std::make_unique< std::vector<MaterialType> >();

		m_material_types->reserve(m_material_settings.size());
		for (auto& settings : m_material_settings)
		{
			m_material_types.get()->emplace_back(settings);
		}



		//Materials
		m_materials.reserve(Materials::MaxCompleteMaterials);
		
		m_materials.emplace_back(std::make_unique<Material>(getMaterialType(MaterialTypes::BillboardMaterial), 
					MaterialData({AssetLoader::loadTextureFromFile("..\\..\\assets\\EditorAssets\\UITextures\\BillboardTextureAtlas.png")})) );



		//TODO meshes

		m_meshes->reserve(Meshes::MaxPreBuiltMeshes);

		MeshData billboard_data;
		fillBillboardMeshData(billboard_data);
		m_meshes->emplace_back(billboard_data);
	}


	MaterialType* EngineResources::getMaterialType(MaterialTypes type)
	{
		return &m_material_types->at(type);
	}

	Material* EngineResources::getMaterial(Materials type)
	{
		return m_materials.at(type).get();
	}

	Mesh* EngineResources::getMesh(Meshes type)
	{
		return &m_meshes->at(type);
	}


}