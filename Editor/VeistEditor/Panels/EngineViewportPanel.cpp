#include <pch.h>
#include "EngineViewportPanel.h"

#include "VeistEditor/EditorApp.h"

//TEMPORARY, replace with renderer alter that uses the rendergraph
#include "Veist/RenderGraph/RenderGraph.h"
#include "Veist/Scenes/ECS/Components/Components.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
//#include "Veist/RenderGraph/RenderGraphPassBuilder.h"

namespace VeistEditor
{

	//TODO: move to renderer when rendergraph tests finished
	static LightProbeComponent* getLightProbe(ecs::EntityRegistry* registry)
	{
		LightProbeComponent* light_probe{nullptr};
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




	
	float EngineViewportPanel::m_aspect_ratio = 16.0f/9.0f;

	EngineViewportPanel::EngineViewportPanel()
	{
		//TODO: image props should be the size of the imgui window
		ImageProperties img_props = ImageProperties({ m_viewport_size }, {VK_FORMAT_R8G8B8A8_SRGB});
		ImageProperties depth_img_props = ImageProperties({ m_viewport_size }, { VK_FORMAT_D32_SFLOAT });

		m_resource_pool = std::make_shared<RenderGraphResourcePool>();

		m_framebuffer_image = std::make_unique<ColorTextureAttachment>(img_props);
		m_depth_image = std::make_unique<DepthTextureAttachment>(depth_img_props);


		//Create framebuffer (only using 1 image for now)
		auto colors = std::vector<Framebuffer::Attachment>{{m_framebuffer_image.get(), RenderPass::LoadOp::Clear}};
		auto depth = Framebuffer::Attachment(m_depth_image.get(), RenderPass::LoadOp::Clear);
		m_target_framebuffer = Framebuffer(colors, depth);
	


		m_texture_id = ImGui_ImplVulkan_AddTexture(Sampler(SamplerType::RepeatLinear).sampler(), m_framebuffer_image.get()->imageView(), getImageLayout(m_framebuffer_image.get()->imageUsage()));
	
		
		m_active_scene = EditorApp::get().getActiveScene();

		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());
	

		
	}

	EngineViewportPanel::~EngineViewportPanel()
	{

	}


	void EngineViewportPanel::onEvent(Event& event)
	{
		EventHandler handler(event);

		handler.handleEvent<EditorSceneChangedEvent>(VEIST_EVENT_BIND_FUNCTION(EngineViewportPanel::changeScene));
		
		if (m_viewport_focused)
		{
			m_editor_camera->onEvent(event);
		}
	}


	void EngineViewportPanel::onDrawPanel()
	{

		update();

		renderPanel();

	}


	void EngineViewportPanel::update()
	{

		if(m_viewport_focused)
		{
			m_editor_camera->onUpdate(EditorApp::get().getFrametime());
		}

	}


	void EngineViewportPanel::renderPanel()
	{
	
		struct PanelConstraint
		{
			static void Square(ImGuiSizeCallbackData* data)
			{
				data->DesiredSize.x = data->DesiredSize.y * m_aspect_ratio;
				//data->DesiredSize.y = data->DesiredSize.x * (1.0f/m_aspect_ratio);
			}
		};
		ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, FLT_MAX), PanelConstraint::Square);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", nullptr);


		renderScene();


		//Check viewport is focused
		if (m_viewport_hovered && !m_viewport_focused && ImGui::IsMouseDown(1))
		{
			ImGui::SetWindowFocus();
		}
		m_viewport_focused = ImGui::IsWindowFocused();
		m_viewport_hovered = ImGui::IsWindowHovered();
		

		ImVec2 viewport_panel_size = ImGui::GetContentRegionAvail();
		viewport_panel_size.x = viewport_panel_size.y * m_aspect_ratio;
		m_viewport_size = { viewport_panel_size.x, viewport_panel_size.y };
		ImGui::Image(m_texture_id, viewport_panel_size);
		ImGui::End();
		ImGui::PopStyleVar();
	
	}

	void EngineViewportPanel::renderScene()
	{
		if (!m_active_scene->ecsRegistry()->isSceneLoaded())
		{
			return;
		}
		//RenderGraph Tests TODO: move declaring rendergraph passes etc to a renderer class or pre declared renderer structure
		/*static constexpr uint32_t max_dir_lights = 4;
		static constexpr uint32_t max_point_lights = 100;

		RenderGraph render_graph(m_resource_pool);

		auto swapchain_image_count = RenderModule::getBackend()->getSwapchainImageCount();

		RenderGraphBufferInfo camera_buffer_info, scene_info_buffer_info, dir_lights_buffer_info, point_lights_buffer_info;

		camera_buffer_info.size = sizeof(RendererUniforms::CameraData);
		scene_info_buffer_info.size = sizeof(RendererUniforms::SceneInfo);
		dir_lights_buffer_info.size = sizeof(RendererUniforms::GPUDirLight) * max_dir_lights;
		point_lights_buffer_info.size = sizeof(RendererUniforms::GPUDirLight) * max_point_lights;

		camera_buffer_info.subbuffer_count = swapchain_image_count;
		scene_info_buffer_info.subbuffer_count = swapchain_image_count;
		dir_lights_buffer_info.subbuffer_count = swapchain_image_count;
		point_lights_buffer_info.subbuffer_count = swapchain_image_count;
		
		RenderGraphImageInfo output_image_info, depth_attachment_info;

		output_image_info.properties = ImageProperties({ m_viewport_size }, { VK_FORMAT_R8G8B8A8_SRGB });
		depth_attachment_info.properties = ImageProperties({ m_viewport_size }, { VK_FORMAT_D32_SFLOAT });


		//Get light probe
		auto light_probe = getLightProbe(m_active_scene->ecsRegistry());//temp static function
		
		
		auto builder = render_graph.addPass("ForwardPass");
		//auto builder2 = render_graph.addPass("ForwardPass2");



		auto camera_buffer = builder.addUniformInput("CameraBuffer", camera_buffer_info);
		auto scene_info_buffer = builder.addUniformInput("SceneInfo", scene_info_buffer_info);
		auto dir_lights_buffer = builder.addUniformInput("DirLights", dir_lights_buffer_info);
		
		auto point_lights_buffer = builder.addStorageInput("PointLights", point_lights_buffer_info);

		builder.addExternalInput("IBLProbe_irrmap", Descriptor(*light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_prefilt_map", Descriptor(*light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
		builder.addExternalInput("IBLProbe_brdfLUT", Descriptor(*light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
		
		auto output_image = builder.addColorOutput("FinalImage", output_image_info);
		auto depth_output = builder.addDepthOutput("DepthAttachment", depth_attachment_info);
		
		

		render_graph.setBackbuffer("FinalImage");
		//End rendergraph tests


		
		builder.setRenderFunction([=](CommandBuffer& cmd, const RenderGraphPass* pass){
		
			auto ecs_registry = m_active_scene->ecsRegistry();
			uint32_t frame_num = RenderModule::getBackend()->getSwapchainImageNumber();
			MatrixPushConstant push_constant;


			//Material* last_material = nullptr;
			//GraphicsPipeline* last_pipeline = nullptr;

			//Camera
			Camera* main_cam;
			{
				auto& scene_view = ecs_registry->view<CameraComponent>();
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

				pass->getPhysicalBuffer(camera_buffer)->setData(&camera_data, sizeof(camera_data), frame_num);
			}


			RendererUniforms::SceneInfo scene_info_data;

			//Global light probe for IBL
			LightProbeComponent* global_light_probe;
			{
				auto& scene_view = ecs_registry->view<LightProbeComponent>();
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
				auto& scene_view = ecs_registry->view<PointLightComponent, TransformComponent>();
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

				pass->getPhysicalBuffer(point_lights_buffer)->setData(&point_lights, sizeof(RendererUniforms::GPUPointLight) * max_point_lights, frame_num);
				scene_info_data.point_lights_count = light_count;
			}

			//Fill directional light buffer
			{

				RendererUniforms::GPUDirLight directional_lights[max_dir_lights];
				uint32_t light_count = 0;
				auto& scene_view = ecs_registry->view<DirectionalLightComponent>();
				glm::mat4 world2view_mat = glm::mat4(glm::mat3(main_cam->viewMatrix()));
				for (ecs::EntityId entity : scene_view)
				{
					auto& light_comp = scene_view.get<DirectionalLightComponent>(entity);

					directional_lights[light_count].colour = light_comp.colour();
					directional_lights[light_count].intensity = light_comp.intensity();
					directional_lights[light_count].direction = world2view_mat * glm::vec4(light_comp.direction(), 1.0);
					light_count++;
				}
				pass->getPhysicalBuffer(dir_lights_buffer)->setData(&directional_lights, sizeof(RendererUniforms::GPUDirLight) * max_dir_lights, frame_num);
				scene_info_data.dir_lights_count = light_count;
			}


			//Scene info buffer
			{
				pass->getPhysicalBuffer(scene_info_buffer)->setData(&scene_info_data, sizeof(RendererUniforms::SceneInfo), frame_num);
			}



			//Render each entity
			auto& scene_view = ecs_registry->view<MeshComponent, TransformComponent>();
			for (ecs::EntityId entity : scene_view)
			{
				auto& mesh_comp = scene_view.get<MeshComponent>(entity);
				auto& transform_comp = scene_view.get<TransformComponent>(entity);

				Mesh* curr_mesh = mesh_comp.mesh();
				Material* curr_material = mesh_comp.material();


				//Model matrices
				push_constant.mat1 = transform_comp.getTransform();
				push_constant.mat2 = glm::inverseTranspose(glm::mat3(main_cam->viewMatrix() * push_constant.mat1));


				//Write directional lights to buffer

				//Create Global descriptor set for that frame if it doenst exists aready
				if (m_global_descriptor[frame_num].descriptorSet() == nullptr)
				{
					m_global_descriptor[frame_num].setDescriptorSetLayout(0, curr_pipeline);

					m_global_descriptor[frame_num].bindUniformBuffer(0, m_scene_info_buffer.get(), sizeof(SceneInfo));
					m_global_descriptor[frame_num].bindUniformBuffer(1, m_camera_buffer.get(), sizeof(CameraData));
					m_global_descriptor[frame_num].bindUniformBuffer(2, m_dir_lights_buffer.get(), sizeof(GPUDirLight) * MAX_DIR_LIGHTS);
					m_global_descriptor[frame_num].bindStorageBuffer(3, m_point_lights_buffer.get(), sizeof(GPUPointLight) * MAX_POINT_LIGHTS);

					

					m_global_descriptor[frame_num].bindDescriptor(4, Descriptor(*global_light_probe->irradianceMap(), { SamplerType::RepeatLinear }));
					m_global_descriptor[frame_num].bindDescriptor(5, Descriptor(*global_light_probe->prefilterMap(), { SamplerType::RepeatLinear }));
					m_global_descriptor[frame_num].bindDescriptor(6, Descriptor(*global_light_probe->brdfLUT(), { SamplerType::RepeatLinear }));


					m_global_descriptor[frame_num].buildDescriptorSet();
				}
				

				cmd.bindMaterial(*curr_material);
				//cmd_buffer.bindPipeline(*curr_material->getPipeline(cmd_buffer.currentRenderPass()));
				cmd.setPushConstants(push_constant);


				constexpr int offset_count = 4;
				uint32_t offset[offset_count];
				offset[0] = pass->getPhysicalBuffer(scene_info_buffer)->offset() * frame_num;
				offset[1] = pass->getPhysicalBuffer(camera_buffer)->offset() * frame_num;
				offset[2] = pass->getPhysicalBuffer(dir_lights_buffer)->offset() * frame_num;
				offset[3] = pass->getPhysicalBuffer(point_lights_buffer)->offset() * frame_num;


				//cmd.bindDescriptorSet(m_global_descriptor[frame_num], offset_count, offset);


				cmd.bindVertexBuffer(*curr_mesh->getVertexBuffer());
				cmd.bindIndexBuffer(*curr_mesh->getIndexBuffer());

				cmd.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());
			}


			//Render skybox
			{
				auto& scene_view = ecs_registry->view<SkyboxComponent>();
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
		

		*/

		CommandBuffer& cmd_buffer = RenderModule::getBackend()->getCurrentCmdBuffer();



		//render_graph.execute(cmd_buffer);
	
		
		
		cmd_buffer.beginRenderPass(m_target_framebuffer);
		RenderModule::renderScene(cmd_buffer);
		cmd_buffer.endRenderPass();

	}




	void EngineViewportPanel::changeScene(EditorSceneChangedEvent& event)
	{
		m_active_scene = event.getNewScene();
		m_editor_camera = std::make_unique<CameraController>(m_active_scene->getMainCamera());

	}
}
