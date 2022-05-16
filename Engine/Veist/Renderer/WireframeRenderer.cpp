#include <pch.h>
#include "WireframeRenderer.h"

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

	WireframeRenderer WireframeRenderer::createRenderer(RenderGraph::RenderGraph& render_graph, ecs::EntityRegistry* scene_registry, const glm::vec2& size)
	{
		WireframeRenderer wireframe_renderer;

		wireframe_renderer.m_size = size;

		static constexpr uint32_t max_objects = 10000;

		RenderGraph::BufferInfo camera_buffer_info, object_matrices_buffer_info;
		
		
		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		object_matrices_buffer_info.size = sizeof(RendererUniforms::ObjectMatrices) * max_objects;

		RenderGraph::ImageInfo output_image_info, depth_attachment_info;

		output_image_info.properties = ImageProperties(wireframe_renderer.m_size, { VK_FORMAT_R8G8B8A8_SRGB });
		depth_attachment_info.properties = ImageProperties(wireframe_renderer.m_size, { VK_FORMAT_D32_SFLOAT });


		//Get light probe
		auto light_probe = getLightProbe(scene_registry);//temp static function

		auto builder = render_graph.addPass("WireframePass");

		//Input and outputs (descriptors)
		auto camera_buffer = builder.addUniformInput("camera_buffer", camera_buffer_info);
		auto object_matrices_buffer = builder.addStorageInput("object_matrices_buffer", object_matrices_buffer_info);

		
		auto output_image = builder.addColorOutput("wireframe_output", output_image_info);
		auto depth_output = builder.addDepthOutput("wireframe_depth_output", depth_attachment_info);


		builder.setRenderGraphImGuiBackbuffer("wireframe_output");
		wireframe_renderer.m_renderer_target = output_image;
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



		
			//Object matrices
			{
				std::vector<RendererUniforms::ObjectMatrices> object_matrices(max_objects);
				auto& scene_view = scene_registry->view<TransformComponent>();
				//uint32_t object_count = 0;
				for (ecs::EntityId entity : scene_view)
				{
					auto& transform_comp = scene_view.get<TransformComponent>(entity);

					uint32_t id = entity;
					object_matrices[entity].model = transform_comp.getTransform();
					object_matrices[entity].normal = glm::inverseTranspose(glm::mat3(main_cam->viewMatrix() * transform_comp.getTransform()));

					//object_count++;
				}
				pass->getPhysicalBuffer(object_matrices_buffer)->setData(object_matrices.data(), sizeof(RendererUniforms::ObjectMatrices) * max_objects);
			}



			cmd.bindMaterialType(EngineResources::MaterialTypes::WireframeMaterial);
			auto& scene_view = scene_registry->view<MeshComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& mesh_comp = scene_view.get<MeshComponent>(entity);

				uint32_t id = entity;
				mesh_comp.renderMesh(cmd, pass->getDescriptorSets(), entity);

			}
			
			//Render skybox
			{
				cmd.bindMaterialType(EngineResources::MaterialTypes::SkyboxMaterial);
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



		
		return wireframe_renderer;
	}




}