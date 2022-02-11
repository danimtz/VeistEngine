#include "pch.h"
#include "ForwardRenderer.h"

#include <GLFW/glfw3.h>

#include "Veist/Scenes/Scene.h"
#include "Veist/Renderer/RenderModule.h"
#include "Veist/Scenes/ECS/Components/Components.h"
#include "Veist/ImGUI/GUIModule.h"//TEMPORARY


namespace Veist
{


void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	/*
	std::unique_ptr<ShaderBuffer> test_buff = std::make_unique<ShaderBuffer>(10,2, ShaderBufferUsage::Uniform); //test buffer suballocations

	test_buff->setData("1234567890", 10, 0);
	test_buff->setData("11681", 3, 1);
	*/


	m_camera_buffer = std::make_unique<ShaderBuffer>(sizeof(CameraData), m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_scene_info_buffer = std::make_unique<ShaderBuffer>(sizeof(SceneInfo), m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_dir_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUDirLight) * MAX_DIR_LIGHTS, m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_point_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUPointLight) * MAX_POINT_LIGHTS, m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Storage);

	m_global_descriptor.resize(m_render_backend->getSwapchain()->imageCount());

}

ForwardRenderer::~ForwardRenderer()
{

}



void ForwardRenderer::setECSRegistry(ecs::EntityRegistry* ecsRegistry)
{
	m_ecs_registry = ecsRegistry;
}



void ForwardRenderer::onUpdate()
{
	auto swapchain = m_render_backend->getSwapchain();
	swapchain->beginNextFrame();

	CommandBuffer& cmd_buffer = m_render_backend->getCurrentCmdBuffer();

	cmd_buffer.begin();



	//Scene Renderpass
	cmd_buffer.beginRenderPass(m_render_backend->getCurrentFramebuffer());

	renderSceneECS(cmd_buffer);

	//cmd_buffer.endRenderPass();


	//GUI renderpass
	//ImageBase* color_attachment = m_render_backend->getSwapchain()->images()[m_render_backend->getSwapchainImageNumber()].get();
	//ImageBase* depth_attachment = m_render_backend->getSwapchainDepthAttachment();
	//Framebuffer fb = { color_attachment , depth_attachment, RenderPass::LoadOp::Load };

	//cmd_buffer.beginRenderPass(fb);
	GUIModule::beginFrame();
	{
		//module or whatever->onUpdateImGUI()
		ImGui::ShowDemoWindow();
	}
	GUIModule::endFrame();


	cmd_buffer.endRenderPass();
	


	//renderer present
	cmd_buffer.end();
	swapchain->present(cmd_buffer);

}


void ForwardRenderer::renderSceneECS(CommandBuffer& cmd_buffer)
{
	
	//Assert m_ecs_registry has been set
	if (m_ecs_registry == nullptr)
	{
		CRITICAL_ERROR_LOG("ECS REGISTRY NOT SET BEFORE ATTEMPTING TO RENDER IT");
	}

	if (!m_ecs_registry->isSceneLoaded())
	{
		return;
	}

	uint32_t frame_num = m_render_backend->getSwapchainImageNumber();
	MatrixPushConstant push_constant;


	Material* last_material = nullptr;
	GraphicsPipeline* last_pipeline = nullptr;



		
	//Camera
	Camera* main_cam;
	{
		auto& scene_view = m_ecs_registry->view<CameraComponent>();
		for (ecs::EntityId entity : scene_view)
		{
			auto& cam_comp = scene_view.get<CameraComponent>(entity);
			main_cam = cam_comp.camera();
			break;//Only first camera componenet being taken into consideration for now
		}
		

		m_camera_data.projection = main_cam->projectionMatrix();
		m_camera_data.view = main_cam->viewMatrix();
		m_camera_data.inverse_view = glm::inverse(glm::mat3(main_cam->viewMatrix()));
		m_camera_data.view_projection = main_cam->viewProjectionMatrix();
		m_camera_buffer->setData(&m_camera_data, sizeof(m_camera_data), frame_num);
	}


	//Global light probe for IBL
	LightProbeComponent* global_light_probe;
	{
		auto& scene_view = m_ecs_registry->view<LightProbeComponent>();
		for (ecs::EntityId entity : scene_view)
		{
			global_light_probe = &scene_view.get<LightProbeComponent>(entity);

			break;//Only first light probe componenet being taken into consideration for now
		}
	}



	//Fill point light buffer
	{

		GPUPointLight point_lights[MAX_POINT_LIGHTS];
	
		uint32_t light_count = 0;
		auto& scene_view = m_ecs_registry->view<PointLightComponent>();
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

		m_point_lights_buffer->setData(&point_lights, sizeof(GPUPointLight) * MAX_DIR_LIGHTS, frame_num);
		m_scene_info.point_lights_count = light_count;
	}

	//Fill directional light buffer
	{

		GPUDirLight directional_lights[MAX_DIR_LIGHTS];
		uint32_t light_count = 0;
		auto& scene_view = m_ecs_registry->view<DirectionalLightComponent>();
		glm::mat4 world2view_mat = glm::mat4(glm::mat3(main_cam->viewMatrix()));
		for (ecs::EntityId entity : scene_view)
		{
			auto& light_comp = scene_view.get<DirectionalLightComponent>(entity);

			directional_lights[light_count].colour = light_comp.colour();
			directional_lights[light_count].intensity = light_comp.intensity();
			directional_lights[light_count].direction = world2view_mat * glm::vec4(light_comp.direction(), 1.0);
			light_count++;
		}
		m_dir_lights_buffer->setData(&directional_lights, sizeof(GPUDirLight) * MAX_DIR_LIGHTS, frame_num);
		m_scene_info.dir_lights_count = light_count;
	}


	//Scene info buffer
	{
		m_scene_info_buffer->setData(&m_scene_info, sizeof(m_scene_info), frame_num);
	}



	//Render each entity
	auto& scene_view = m_ecs_registry->view<MeshComponent>();
	for (ecs::EntityId entity : scene_view)
	{
		auto& mesh_comp = scene_view.get<MeshComponent>(entity);
		auto& transform_comp = scene_view.get<TransformComponent>(entity);

		Mesh* curr_mesh = mesh_comp.mesh();
		Material* curr_material = mesh_comp.material();
		GraphicsPipeline* curr_pipeline = curr_material->pipeline().get();


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

			m_global_descriptor[frame_num].bindCombinedSamplerCubemap(4, global_light_probe->irradianceMap());
			m_global_descriptor[frame_num].bindCombinedSamplerCubemap(5, global_light_probe->prefilterMap());
			m_global_descriptor[frame_num].bindCombinedSamplerTexture(6, global_light_probe->brdfLUT());


			m_global_descriptor[frame_num].buildDescriptorSet();
		}



		//Check if pipeline changed
		if (curr_pipeline != last_pipeline)
		{



			cmd_buffer.bindPipeline(*curr_material->pipeline());
			cmd_buffer.setPushConstants(push_constant);


			constexpr int offset_count = 4;
			uint32_t offset[offset_count];
			offset[0] = m_scene_info_buffer->offset() * frame_num;
			offset[1] = m_camera_buffer->offset() * frame_num;
			offset[2] = m_dir_lights_buffer->offset() * frame_num;
			offset[3] = m_point_lights_buffer->offset() * frame_num;

			cmd_buffer.bindDescriptorSet(*curr_material->pipeline(), m_global_descriptor[frame_num], offset_count, offset);

			//Check if material changed  
			if (curr_material != last_material)
			{
				//Change material descriptors
				//texture descriptor
				cmd_buffer.bindMaterial(*curr_material);
			}
		}
		cmd_buffer.bindVertexBuffer(*curr_mesh->getVertexBuffer());
		cmd_buffer.bindIndexBuffer(*curr_mesh->getIndexBuffer());

		cmd_buffer.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());
	}


	//Render skybox
	{
		auto& scene_view = m_ecs_registry->view<SkyboxComponent>();
		for (ecs::EntityId entity : scene_view)
		{
			auto& skybox_comp = scene_view.get<SkyboxComponent>(entity);

			Material* curr_material = skybox_comp.material();
			Mesh* curr_mesh = skybox_comp.mesh();

			cmd_buffer.bindMaterial(*curr_material);
			MatrixPushConstant push_constant;
			push_constant.mat1 = glm::mat3(main_cam->viewMatrix()); //view without tranlation
			push_constant.mat2 = main_cam->projectionMatrix();	//projection
			cmd_buffer.setPushConstants(push_constant);

			cmd_buffer.bindVertexBuffer(*curr_mesh->getVertexBuffer());
			cmd_buffer.bindIndexBuffer(*curr_mesh->getIndexBuffer());

			cmd_buffer.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());
			
			break;//Only loop once
		}
	}



}












}