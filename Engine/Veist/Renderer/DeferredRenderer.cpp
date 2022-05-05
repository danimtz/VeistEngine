#include <pch.h>
#include "DeferredRenderer.h"

#include "Veist/Scenes/ECS/Components/Components.h"
#include "Veist/Scenes/ECS/EntityRegistry.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace Veist
{
	
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


	static void addGBufferPass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, DeferredRenderer& renderer)
	{
		static constexpr uint32_t max_objects = 10000;

		RenderGraph::BufferInfo camera_buffer_info, object_matrices_buffer_info;
		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		object_matrices_buffer_info.size = sizeof(RendererUniforms::ObjectMatrices) * max_objects;




		RenderGraph::ImageInfo gbuff_albedo_info, gbuff_normal_info, gbuffer_occ_rough_metal_info, gbuff_emmissive_info, depth_attachment_info;
		gbuff_albedo_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });
		gbuff_normal_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_UNORM });
		gbuffer_occ_rough_metal_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_UNORM });
		gbuff_emmissive_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });
		depth_attachment_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_D32_SFLOAT });

		auto builder = render_graph.addPass("GbufferPass");

		auto camera_buffer = builder.addUniformInput("camera_buffer", camera_buffer_info);
		auto object_matrices_buffer = builder.addStorageInput("object_matrices_buffer", object_matrices_buffer_info, PipelineStage::VertexShader, 1);

		builder.addColorOutput("gbuffer_albedo", gbuff_albedo_info);
		auto output_test = builder.addColorOutput("gbuffer_normal", gbuff_normal_info);
		builder.addColorOutput("gbuffer_occ_rough_metal", gbuffer_occ_rough_metal_info);
		//builder.addColorOutput("gbuffer_emmissive", gbuff_emmissive_info);
		builder.addDepthOutput("gbuffer_depth_attachment", depth_attachment_info);
		

		//builder.setRenderGraphImGuiBackbuffer("gbuffer_normal");
		//renderer.m_editor_target = output_test;

		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass) {
			

			Camera* main_cam;
			{
				auto& scene_view = scene_registry->view<CameraComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& cam_comp = scene_view.get<CameraComponent>(entity);
					main_cam = cam_comp.camera();
					break;//Only first camera componenet being taken into consideration for now
				}

				RendererUniforms::CameraData camera_data;
				camera_data.projection = main_cam->projectionMatrix();
				camera_data.view = main_cam->viewMatrix();
				camera_data.inverse_view = glm::inverse(glm::mat3(main_cam->viewMatrix()));
				camera_data.view_projection = main_cam->viewProjectionMatrix();
				camera_data.inverse_projection = glm::inverse(main_cam->projectionMatrix());

				pass->getPhysicalBuffer(camera_buffer)->setData(&camera_data, sizeof(RendererUniforms::CameraData));
			}

			//Object matrices for all entities with a transform
			{
				std::vector<RendererUniforms::ObjectMatrices> object_matrices(max_objects);
				auto& scene_view = scene_registry->view<TransformComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& transform_comp = scene_view.get<TransformComponent>(entity);

					object_matrices[entity].model = transform_comp.getTransform();
					object_matrices[entity].normal = glm::inverseTranspose(glm::mat3(main_cam->viewMatrix() * transform_comp.getTransform()));

					
				}
				pass->getPhysicalBuffer(object_matrices_buffer)->setData(object_matrices.data(), sizeof(RendererUniforms::ObjectMatrices) * max_objects);
			}


			auto& scene_view = scene_registry->view<MeshComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				MatrixPushConstant push_constant;
				auto& mesh_comp = scene_view.get<MeshComponent>(entity);

				mesh_comp.renderMesh(cmd, pass->getDescriptorSets(), entity);
				
			}
			
		});

	}


	static void addLightingPass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, DeferredRenderer& renderer)
	{
		static constexpr uint32_t max_dir_lights = 4;
		static constexpr uint32_t max_point_lights = 100;

		RenderGraph::BufferInfo camera_buffer_info, scene_info_buffer_info, dir_lights_buffer_info, point_lights_buffer_info;

		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		scene_info_buffer_info.size = sizeof(RendererUniforms::SceneInfo);
		dir_lights_buffer_info.size = sizeof(RendererUniforms::GPUDirLight) * max_dir_lights;
		point_lights_buffer_info.size = sizeof(RendererUniforms::GPUPointLight) * max_point_lights;


		RenderGraph::ImageInfo output_image_info;
		output_image_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });

		auto builder = render_graph.addPass("LightingPass");

		
		//Input and outputs (descriptors)
		auto scene_info_buffer = builder.addUniformInput("scene_info", scene_info_buffer_info);
		auto camera_buffer = builder.addUniformInput("camera_buffer");
		auto dir_lights_buffer = builder.addUniformInput("dir_lights", dir_lights_buffer_info);
		auto point_lights_buffer = builder.addStorageInput("point_lights", point_lights_buffer_info);

		//Get light probe
		auto light_probe = getLightProbe(scene_registry);//temp static function

		builder.addExternalInput("IBLProbe_irrmap", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->irradianceMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_prefilt_map", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_brdfLUT", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->brdfLUT(), { SamplerType::ClampLinear }));

		builder.addTextureInput("gbuffer_albedo");
		builder.addTextureInput("gbuffer_normal");
		builder.addTextureInput("gbuffer_occ_rough_metal");
		//builder.addTextureInput("gbuffer_emmissive");

		builder.addTextureInput("gbuffer_depth_attachment");//TODO adding these two lines should create layout DEPTH_STENCIL_READ_ONLY_OPTIMAL
		builder.addDepthInput("gbuffer_depth_attachment");

		

		auto lighting_output = builder.addColorOutput("lighting_output", output_image_info);

		
		//builder.setRenderGraphBackbuffer("lighting_output");
		//builder.setRenderGraphImGuiBackbuffer("lighting_output");
		//renderer.m_editor_target = lighting_output;



		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass) {

		
			Camera* main_cam;
			{
				auto& scene_view = scene_registry->view<CameraComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& cam_comp = scene_view.get<CameraComponent>(entity);
					main_cam = cam_comp.camera();
					break;//Only first camera componenet being taken into consideration for now
				}

				/*RendererUniforms::CameraData camera_data;
				camera_data.projection = main_cam->projectionMatrix();
				camera_data.view = main_cam->viewMatrix();
				camera_data.inverse_view = glm::inverse(glm::mat3(main_cam->viewMatrix()));
				camera_data.view_projection = main_cam->viewProjectionMatrix();
				camera_data.inverse_projection = glm::inverse(main_cam->projectionMatrix());

				pass->getPhysicalBuffer(camera_buffer)->setData(&camera_data, sizeof(RendererUniforms::CameraData));*/
			}
		

			RendererUniforms::SceneInfo scene_info_data;

			//Fill point light buffer
			{

				RendererUniforms::GPUPointLight point_lights[max_point_lights];
				uint32_t light_count = 0;
				auto& scene_view = scene_registry->view<PointLightComponent, TransformComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& light_comp = scene_view.get<PointLightComponent>(entity);
					auto& transform_comp = scene_view.get<TransformComponent>(entity);

					point_lights[light_count].colour = light_comp.colour();
					point_lights[light_count].intensity = light_comp.intensity();
					point_lights[light_count].radius = light_comp.radius();
					point_lights[light_count].position = main_cam->viewMatrix() * glm::vec4(transform_comp.translation(), 1.0);

					light_count++;
				}

				pass->getPhysicalBuffer(point_lights_buffer)->setData(&point_lights, sizeof(RendererUniforms::GPUPointLight) * max_point_lights);
				scene_info_data.point_lights_count = light_count;
			}


			//Fill directional light buffer
			{

				RendererUniforms::GPUDirLight directional_lights[max_dir_lights];
				uint32_t light_count = 0;
				auto& scene_view = scene_registry->view<DirectionalLightComponent, TransformComponent>();
				glm::mat4 world2view_mat = glm::mat4(glm::mat3(main_cam->viewMatrix()));
				for (ecs::EntityId entity : scene_view)
				{
					auto& light_comp = scene_view.get<DirectionalLightComponent>(entity);

					directional_lights[light_count].colour = light_comp.colour();
					directional_lights[light_count].intensity = light_comp.intensity();
					directional_lights[light_count].direction = world2view_mat * glm::vec4(light_comp.direction(), 1.0);
					light_count++;
				}
				pass->getPhysicalBuffer(dir_lights_buffer)->setData(&directional_lights, sizeof(RendererUniforms::GPUDirLight) * max_dir_lights);
				scene_info_data.dir_lights_count = light_count;
			}
			
			//Scene info buffer
			{
				pass->getPhysicalBuffer(scene_info_buffer)->setData(&scene_info_data, sizeof(RendererUniforms::SceneInfo));
			}

			cmd.bindMaterialType(EngineResources::MaterialTypes::DeferredLightingMaterial);

			cmd.bindDescriptorSet(pass->getDescriptorSets()[0]);

			cmd.drawVertices(3);

		});


	}


	static void addSkyboxPass(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, DeferredRenderer& renderer)
	{
		RenderGraph::ImageInfo output_image_info;
		output_image_info.properties = ImageProperties(renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });

		auto builder = render_graph.addPass("SkyboxPass");

		builder.addDepthInput("gbuffer_depth_attachment");
		auto output_image = builder.addColorOutput("renderer_output", output_image_info, "lighting_output");
		
		//outputs of rendergraph
		builder.setRenderGraphImGuiBackbuffer("renderer_output");

		renderer.m_renderer_target = output_image;

		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass)
		{
			//Get camera
			Camera* main_cam;
			{
				auto& scene_view = scene_registry->view<CameraComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& cam_comp = scene_view.get<CameraComponent>(entity);
					main_cam = cam_comp.camera();
					break;//Only first camera componenet being taken into consideration for now
				}
			}
			//Render skybox
			{
				auto& scene_view = scene_registry->view<SkyboxComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					auto& skybox_comp = scene_view.get<SkyboxComponent>(entity);

					Material* curr_material = skybox_comp.material();
					Mesh* curr_mesh = skybox_comp.mesh();

					cmd.bindMaterial(*curr_material);
					MatrixPushConstant push_constant;
					push_constant.mat1 = glm::mat3(main_cam->viewMatrix()); //view without tranlation
					push_constant.mat2 = main_cam->projectionMatrix();	//projection
					cmd.setPushConstants(push_constant);

					cmd.bindVertexBuffer(*curr_mesh->getVertexBuffer());
					cmd.bindIndexBuffer(*curr_mesh->getIndexBuffer());

					cmd.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());

					break;//Only loop once
				}
			}
		
		});


	}


	DeferredRenderer DeferredRenderer::createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size)
	{
		DeferredRenderer deferred_renderer;

		deferred_renderer.m_size = size;

		addGBufferPass(render_graph, scene_registry, deferred_renderer);
		addLightingPass(render_graph, scene_registry, deferred_renderer);
		addSkyboxPass(render_graph, scene_registry, deferred_renderer);




		return deferred_renderer;
	}




}