#include <pch.h>
#include "EditorRenderer.h"

#include "Veist/Scenes/ECS/Components/Components.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>


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

				uv_ranges.uv1uv2.z = 0.5f;
				uv_ranges.uv1uv2.a = 1.0f;
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

		RenderGraph::ImageInfo output_image_info;
		output_image_info.properties = ImageProperties(renderer.m_renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });

		auto builder = render_graph.addPass("EditorPass");

		//builder.addDepthInput("gbuffer_depth_attachment");
		auto output_image = builder.addColorOutput("editor_output", output_image_info, "renderer_output");

		//outputs of rendergraph
		builder.setRenderGraphImGuiBackbuffer("editor_output");

		renderer.m_editor_target = output_image;



		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass)
		{
			

			//PointLights
			auto& scene_view = scene_registry->view<PointLightComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& light_comp = scene_view.get<PointLightComponent>(entity);
				auto& transform_comp = scene_view.get<TransformComponent>(entity);

				//TODO render billboard
				

				auto material = RenderModule::resources()->getMaterial(EngineResources::Materials::EditorBillboardIcons);
				cmd.bindMaterial(*material);

				auto mesh = RenderModule::resources()->getMesh(EngineResources::Meshes::BillboardMesh);
				cmd.bindVertexBuffer(*mesh->getVertexBuffer());
				cmd.bindIndexBuffer(*mesh->getIndexBuffer());


				BillboardUVRanges uv_ranges = getBillboardUV(BillboardType::PointLight);

				cmd.setPushConstants(&uv_ranges, sizeof(BillboardUVRanges));

				cmd.drawIndexed(mesh->getIndexBuffer()->getIndexCount());
				//cmd.drawVertices(*m_mesh->getIndexBuffer());
			}
			
			
			
		});


	}


	void addSelectionOutlinePass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, EditorRenderer& renderer)
	{

	}


	EditorRenderer EditorRenderer::createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size)
	{
		EditorRenderer editor_renderer;

		editor_renderer.m_render_graph = &render_graph;
		
		//editor_renderer.m_renderer = DeferredRenderer::createRenderer(render_graph, scene_registry, size);
		editor_renderer.m_renderer = BasicRenderer::createRenderer(render_graph, scene_registry, size);
		
		addEditorBillboardPass(render_graph, scene_registry, editor_renderer);

		editor_renderer.m_editor_target = editor_renderer.m_renderer.m_renderer_target;

		return editor_renderer;
	}






	void* EditorRenderer::getImGuiTextureId()
	{
		std::vector<Descriptor> descriptor;
		ImageBase* target = m_render_graph->resourcePool()->getImage(static_cast<RenderGraph::PhysicalImage*>(m_editor_target->physicalResource()))->image.get();
		descriptor.emplace_back(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, target, SamplerType::RepeatLinear, VK_SHADER_STAGE_FRAGMENT_BIT);

		m_imgui_texture_id = std::make_unique<DescriptorSet>( 0, descriptor );

		return m_imgui_texture_id->descriptorSet();
	}



}