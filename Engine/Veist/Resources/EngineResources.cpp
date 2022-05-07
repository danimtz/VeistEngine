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
		case EngineResources::MaterialTypes::EditorTargetSelectMaterial: //Quad
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

		
		std::vector<Vertex>& vertices = mesh_data.vbuffer_data;
		vertices.resize(4);

		vertices[0].position = glm::vec3{ -0.5, -0.5, 0.0 };
		vertices[1].position = glm::vec3{ 0.5, -0.5, 0.0 };
		vertices[2].position = glm::vec3{ -0.5, 0.5, 0.0 };
		vertices[3].position = glm::vec3{ 0.5, 0.5, 0.0 };


		mesh_data.vbuffer_size = mesh_data.vbuffer_data.size() * mesh_data.description.getStride();

		mesh_data.index_count = 6;
		uint16_t indices[36] = { 0, 1, 2, 2, 1, 3};

		mesh_data.index_data.resize(mesh_data.index_count);
		memcpy(mesh_data.index_data.data(), indices, mesh_data.index_count * mesh_data.index_size);


		
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
		DeferredLighting.vertex_shader_name = "FullScreenQuad.vert";
		DeferredLighting.fragment_shader_name = "DeferredLighting.frag";
		DeferredLighting.depth_setting = DepthTest::Read;


		MaterialSettings EditorBillboard = {};
		EditorBillboard.vertex_description = getVertexDescription(EngineResources::MaterialTypes::BillboardMaterial); //This is for vertex description generation
		EditorBillboard.descriptor_set_number = 1;
		EditorBillboard.attachment_count = 1;
		EditorBillboard.vertex_shader_name = "CameraViewBillboard.vert";
		EditorBillboard.fragment_shader_name = "CameraViewBillboard.frag";
		EditorBillboard.depth_setting = DepthTest::Read;


		MaterialSettings EditorTargetSelect = {};
		EditorTargetSelect.vertex_description = getVertexDescription(EngineResources::MaterialTypes::EditorTargetSelectMaterial); //This is for vertex description generation
		EditorTargetSelect.descriptor_set_number = 1;
		EditorTargetSelect.attachment_count = 1;
		EditorTargetSelect.vertex_shader_name = "FullScreenQuad.vert";
		EditorTargetSelect.fragment_shader_name = "EditorTargetSelect.frag";
		EditorTargetSelect.depth_setting = DepthTest::Read;

		material_settings.emplace_back(PBRForward);
		material_settings.emplace_back(Skybox);
		material_settings.emplace_back(GBuffer);
		material_settings.emplace_back(DeferredLighting);
		material_settings.emplace_back(EditorBillboard);
		material_settings.emplace_back(EditorTargetSelect);
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



		m_meshes.reserve(Meshes::MaxPreBuiltMeshes);

		MeshData billboard_data;
		fillBillboardMeshData(billboard_data);

		m_meshes.emplace_back(std::make_unique<Mesh>(billboard_data));
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
		return m_meshes.at(type).get();
	}


}