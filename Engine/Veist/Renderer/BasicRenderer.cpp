#include <pch.h>
#include "BasicRenderer.h"

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

	BasicRenderer BasicRenderer::createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry)
	{
		BasicRenderer basic_renderer;

		static constexpr uint32_t max_dir_lights = 4;
		static constexpr uint32_t max_point_lights = 100;

		RenderGraph::BufferInfo camera_buffer_info, scene_info_buffer_info, dir_lights_buffer_info, point_lights_buffer_info;

		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		scene_info_buffer_info.size = sizeof(RendererUniforms::SceneInfo);
		dir_lights_buffer_info.size = sizeof(RendererUniforms::GPUDirLight) * max_dir_lights;
		point_lights_buffer_info.size = sizeof(RendererUniforms::GPUPointLight) * max_point_lights;

		RenderGraph::ImageInfo output_image_info, depth_attachment_info;

		output_image_info.properties = ImageProperties({ 1920,1080 }, { VK_FORMAT_R8G8B8A8_SRGB });
		depth_attachment_info.properties = ImageProperties({ 1920,1080 }, { VK_FORMAT_D32_SFLOAT });


		//Get light probe
		auto light_probe = getLightProbe(scene_registry);//temp static function

		auto builder = render_graph.addPass("ForwardPass");

		//Input and outputs (descriptors)
		auto scene_info_buffer = builder.addUniformInput("SceneInfo", scene_info_buffer_info);
		auto camera_buffer = builder.addUniformInput("CameraBuffer", camera_buffer_info);
		auto dir_lights_buffer = builder.addUniformInput("DirLights", dir_lights_buffer_info);
		auto point_lights_buffer = builder.addStorageInput("PointLights", point_lights_buffer_info);

		builder.addExternalInput("IBLProbe_irrmap", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->irradianceMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_prefilt_map", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_brdfLUT", Descriptor(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, light_probe->brdfLUT(), { SamplerType::RepeatLinear }));

		auto output_image = builder.addColorOutput("Color", output_image_info);
		auto depth_output = builder.addDepthOutput("Depth", depth_attachment_info);


		basic_renderer.m_editor_target = output_image;


		render_graph.setBackbuffer("Color");

		//End rendergraph tests


		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraph::RenderGraphPass* pass) {

			
			uint32_t frame_num = RenderModule::getBackend()->getSwapchainImageNumber();
			MatrixPushConstant push_constant;


			//Camera
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


			RendererUniforms::SceneInfo scene_info_data;

			//Global light probe for IBL
			LightProbeComponent* global_light_probe;
			{
				auto& scene_view = scene_registry->view<LightProbeComponent>();
				for (ecs::EntityId entity : scene_view)
				{
					global_light_probe = &scene_view.get<LightProbeComponent>(entity);

					break;//Only first light probe componenet being taken into consideration for now
				}
			}


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
				auto& scene_view = scene_registry->view<DirectionalLightComponent>();
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


			//Render each entity
			auto& scene_view = scene_registry->view<MeshComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& mesh_comp = scene_view.get<MeshComponent>(entity);
				auto& transform_comp = scene_view.get<TransformComponent>(entity);

				Mesh* curr_mesh = mesh_comp.mesh();
				Material* curr_material = mesh_comp.material();


				//Model matrices
				push_constant.mat1 = transform_comp.getTransform();
				push_constant.mat2 = glm::inverseTranspose(glm::mat3(main_cam->viewMatrix() * push_constant.mat1));



				cmd.bindMaterial(*curr_material);
				cmd.setPushConstants(push_constant);

				cmd.bindDescriptorSet(pass->getDescriptorSets()[0]);

				cmd.bindVertexBuffer(*curr_mesh->getVertexBuffer());
				cmd.bindIndexBuffer(*curr_mesh->getIndexBuffer());

				cmd.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());
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



		
		return basic_renderer;
	}




}