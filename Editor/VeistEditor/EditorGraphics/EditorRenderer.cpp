#include <pch.h>
#include "EditorRenderer.h"

#include "Veist/Scenes/ECS/Components/Components.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "Veist/Renderer/DeferredRenderer.h"
#include "Veist/Renderer/BasicRenderer.h"
#include "Veist/Renderer/WireframeRenderer.h"

namespace Veist
{
	
	

	struct BillboardUVRanges
	{
		glm::vec4 uv1uv2 = {};
	};

	enum class BillboardType
	{
		PointLight,
		DirectionalLight,
		Camera
	};

	BillboardUVRanges getBillboardUV(BillboardType type)
	{
		BillboardUVRanges uv_ranges;
		switch (type)
		{
			case BillboardType::PointLight:
				uv_ranges.uv1uv2.x = 0.0f;
				uv_ranges.uv1uv2.y = 0.0f;

				uv_ranges.uv1uv2.z = 0.125f;
				uv_ranges.uv1uv2.a = 0.125f;
				break;

			case BillboardType::DirectionalLight:
				uv_ranges.uv1uv2.x = 0.125f;
				uv_ranges.uv1uv2.y = 0.0f;

				uv_ranges.uv1uv2.z = 0.25f;
				uv_ranges.uv1uv2.a = 0.125f;
				break;

			case BillboardType::Camera:
				uv_ranges.uv1uv2.x = 0.0f;
				uv_ranges.uv1uv2.y = 0.125f;

				uv_ranges.uv1uv2.z = 0.125f;
				uv_ranges.uv1uv2.a = 0.25f;

				break;
			default:
				uv_ranges.uv1uv2.x = 0.0f;
				uv_ranges.uv1uv2.y = 0.0f;

				uv_ranges.uv1uv2.z = 1.0f;
				uv_ranges.uv1uv2.a = 1.0f;
				break; 
		}

		return uv_ranges;
	}
	static LightProbeComponent* getLightProbe(ecs::EntityRegistry* registry)
	{
		LightProbeComponent* light_probe{ nullptr };
		{
			auto& scene_view = registry->view<LightProbeComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				light_probe = &scene_view.get<LightProbeComponent>(entity);

				break;//Only first light probe componenet being taken into consideration for now
			}
		}
		return light_probe;
	}




	void addEditorBillboardPass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, EditorRenderer& renderer)
	{
		RenderGraph::BufferInfo camera_buffer_info;

		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		
		RenderGraph::ImageInfo output_image_info;
		output_image_info.properties = ImageProperties(renderer.m_renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });

		auto builder = render_graph.addPass("EditorPass");

		auto camera_buffer = builder.addUniformInput("camera_buffer");
		builder.addStorageInput("object_matrices_buffer");
		auto output_image = builder.addColorOutput("editor_billboard_output", output_image_info, "renderer_output");
		builder.addDepthInput("gbuffer_depth_attachment");//builder.addDepthInput("depth_output");

		//outputs of rendergraph
		builder.setRenderGraphImGuiBackbuffer("editor_billboard_output");
		renderer.m_editor_target = output_image;



		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass)
		{
			
			uint32_t object_matrix_id = 0;


			cmd.bindMaterialType(EngineResources::MaterialTypes::BillboardMaterial);

			auto billboard_material = RenderModule::resources()->getMaterial(EngineResources::Materials::EditorBillboardIcons);
			cmd.bindMaterial(*billboard_material);

			auto billboard_mesh = RenderModule::resources()->getMesh(EngineResources::Meshes::BillboardMesh);
			cmd.bindVertexBuffer(*billboard_mesh->getVertexBuffer());
			cmd.bindIndexBuffer(*billboard_mesh->getIndexBuffer());

			for (auto& descriptor_set : pass->getDescriptorSets())
			{
				cmd.bindDescriptorSet(descriptor_set);
			}

			// Render Directional light icons
			{
				BillboardUVRanges uv_ranges = getBillboardUV(BillboardType::DirectionalLight);
				cmd.setPushConstants(&uv_ranges, sizeof(BillboardUVRanges));

				auto& scene_view = scene_registry->view<DirectionalLightComponent, TransformComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					cmd.drawIndexed(billboard_mesh->getIndexBuffer()->getIndexCount(), entity);
				}
			}


			//Render point light icons
			{
				BillboardUVRanges uv_ranges = getBillboardUV(BillboardType::PointLight);
				cmd.setPushConstants(&uv_ranges, sizeof(BillboardUVRanges));
				
				auto& scene_view = scene_registry->view<PointLightComponent, TransformComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					cmd.drawIndexed(billboard_mesh->getIndexBuffer()->getIndexCount(), entity);
				}
			}
			

			//Render camera icons
			{
				BillboardUVRanges uv_ranges = getBillboardUV(BillboardType::Camera);
				cmd.setPushConstants(&uv_ranges, sizeof(BillboardUVRanges));

				auto& scene_view = scene_registry->view<CameraComponent, TransformComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					cmd.drawIndexed(billboard_mesh->getIndexBuffer()->getIndexCount(), entity);
				}
			}
			
		});


	}


	void addSelectionOutlinePass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, EditorRenderer& renderer)
	{

	}



	void addTargetSelectionPass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, EditorRenderer& renderer, uint32_t target_view)
	{
		RenderGraph::ImageInfo output_image_info;
		output_image_info.properties = ImageProperties(renderer.m_renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });

		auto builder = render_graph.addPass("TargetSelectionPass");

		builder.addTextureInput("gbuffer_albedo");
		builder.addTextureInput("gbuffer_normal");
		builder.addTextureInput("gbuffer_occ_rough_metal");
		builder.addTextureInput("gbuffer_depth_attachment");

		auto output_image = builder.addColorOutput("editor_output", output_image_info, "editor_billboard_output");

		builder.setRenderGraphImGuiBackbuffer("editor_output");
		renderer.m_editor_target = output_image;


		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass)
		{

			cmd.bindMaterialType(EngineResources::MaterialTypes::EditorTargetSelectMaterial);
			
			for (auto& descriptor_set : pass->getDescriptorSets())
			{
				cmd.bindDescriptorSet(descriptor_set);
			}

			cmd.setPushConstants(&target_view, sizeof(uint32_t));
			cmd.drawVertices(3);
			
		});

	}






	EditorRenderer EditorRenderer::createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size, uint32_t view_target, uint32_t fill_type, uint32_t renderer_type)
	{
		EditorRenderer editor_renderer;

		editor_renderer.m_render_graph = &render_graph;
		
		
		if (fill_type == VK_POLYGON_MODE_FILL)
		{

			if (renderer_type == 1)
			{
				editor_renderer.m_renderer = DeferredRenderer::createRenderer(render_graph, scene_registry, size);
				addEditorBillboardPass(render_graph, scene_registry, editor_renderer);
				addTargetSelectionPass(render_graph, scene_registry, editor_renderer, view_target);

			}
			else if (renderer_type == 0)
			{
				editor_renderer.m_renderer = BasicRenderer::createRenderer(render_graph, scene_registry, size);
				addEditorBillboardPass(render_graph, scene_registry, editor_renderer);
			}
			else
			{
				CRITICAL_ERROR_LOG("Unknown renderer type");
			}
		
		}
		else
		{
			editor_renderer.m_renderer = WireframeRenderer::createRenderer(render_graph, scene_registry, size);
		}
		

		//todo ADD SELECTION OUTLINE PASS

		//editor_renderer.m_renderer = BasicRenderer::createRenderer(render_graph, scene_registry, size);
		


		

		

		

		editor_renderer.m_editor_target = editor_renderer.m_renderer.m_renderer_target;

		return editor_renderer;
	}






	void* EditorRenderer::getImGuiTextureId()
	{
		std::vector<Descriptor> descriptor;
		ImageBase* target = m_render_graph->resourcePool()->getImage(static_cast<RenderGraph::PhysicalImage*>(m_editor_target->physicalResource()))->image.get();
		descriptor.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, target, SamplerType::RepeatLinear, VK_SHADER_STAGE_FRAGMENT_BIT);

		m_imgui_texture_id = std::make_unique<DescriptorSet>( descriptor, 0 );

		return m_imgui_texture_id->descriptorSet();
	}



}