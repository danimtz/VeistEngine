#include "ForwardRenderer.h"

#include <GLFW/glfw3.h>

#include "Engine/Scenes/Scene.h"
#include "Engine/Renderer/RenderModule.h"

#include "Engine/ImGUI/GUIModule.h"//TEMPORARY



void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	std::unique_ptr<ShaderBuffer> test_buff = std::make_unique<ShaderBuffer>(10,2, ShaderBufferUsage::Uniform); //test buffer suballocations

	
	

	test_buff->setData("1234567890", 10, 0);
	test_buff->setData("11681", 3, 1);



	m_camera_buffer = std::make_unique<ShaderBuffer>(sizeof(CameraData), m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_scene_info_buffer = std::make_unique<ShaderBuffer>(sizeof(SceneInfo), m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_dir_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUDirLight) * MAX_DIR_LIGHTS, m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Uniform);
	m_point_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUPointLight) * MAX_POINT_LIGHTS, m_render_backend->getSwapchain()->imageCount(), ShaderBufferUsage::Storage);

	m_global_descriptor.resize(m_render_backend->getSwapchain()->imageCount());

}

ForwardRenderer::~ForwardRenderer()
{

}


void ForwardRenderer::setScene(Scene* scene)  
{
	m_scene = scene;
}


void ForwardRenderer::onUpdate()
{
	auto swapchain = m_render_backend->getSwapchain();
	swapchain->beginNextFrame();

	CommandBuffer& cmd_buffer = m_render_backend->getCurrentCmdBuffer();

	cmd_buffer.begin();
	cmd_buffer.beginRenderPass(m_render_backend->getCurrentFramebuffer());

	renderScene(cmd_buffer);
	


	GUIModule::beginFrame();
	{
		//module or whatever->onUpdateImGUI()
		ImGui::ShowDemoWindow();
	}
	GUIModule::endFrame();


	cmd_buffer.endRenderPass();
	cmd_buffer.end();

	swapchain->present(cmd_buffer);

}


void ForwardRenderer::renderScene(CommandBuffer& cmd_buffer)
{
	//THIS WILL BE REFACTORED INTO MANY THINGS-> graphics context instead of backend, renderpass and cmd class maybe instead of RC_begin frame
	

	uint32_t frame_counter = m_render_backend->getFrameNumber();
	uint32_t frame_num = m_render_backend->getSwapchainImageNumber();
	MatrixPushConstant push_constant;

	
	Material* last_material = nullptr;
	GraphicsPipeline* last_pipeline = nullptr;


	//Write per-frame buffers (camera, lights)
	m_camera_data.projection = m_scene->getCamera()->projectionMatrix();
	m_camera_data.view = m_scene->getCamera()->viewMatrix();
	m_camera_data.view_projection = m_scene->getCamera()->viewProjectionMatrix();
	m_camera_buffer->setData(&m_camera_data, sizeof(m_camera_data), frame_num);

	m_scene_info.dir_lights_count = m_scene->getDirLights().size();
	m_scene_info.point_lights_count = m_scene->getPointLights().size();
	m_scene_info_buffer->setData(&m_scene_info, sizeof(m_scene_info), frame_num);


	//Directional lighting
	if (m_scene_info.dir_lights_count > 0) 
	{
		GPUDirLight dir_lights[MAX_DIR_LIGHTS];
		memcpy(dir_lights, m_scene->getDirLights().data(), sizeof(DirectionalLight)* m_scene_info.dir_lights_count);
		glm::mat4 world_to_view = glm::mat4(glm::mat3(m_scene->getCamera()->viewMatrix()));
		for (int i = 0; i < m_scene_info.dir_lights_count; i++) {
			dir_lights[i].direction = world_to_view * glm::vec4(m_scene->getDirLights()[i].direction(), 1.0);
		}
		m_dir_lights_buffer->setData(&dir_lights, sizeof(GPUDirLight)* MAX_DIR_LIGHTS, frame_num);
	}


	//Point lights
	if (m_scene_info.point_lights_count > 0)
	{
		GPUPointLight point_lights[MAX_POINT_LIGHTS];
		memcpy(point_lights, m_scene->getPointLights().data(), sizeof(PointLight)* m_scene_info.point_lights_count);
		m_scene->getCamera()->viewMatrix();
		for (int i = 0; i < m_scene_info.point_lights_count; i++) {
			point_lights[i].position = m_scene->getCamera()->viewMatrix() * glm::vec4(m_scene->getPointLights()[i].position(), 1.0);
		}
		m_point_lights_buffer->setData(&point_lights, sizeof(GPUDirLight) * MAX_POINT_LIGHTS, frame_num);
	}



	//Render each model
	for (int i = 0; i < m_scene->getModels().size(); i++) 
	{
		
		Mesh* curr_mesh = m_scene->getModels()[i].mesh().get();
		Material* curr_material = m_scene->getModels()[i].material().get();
		GraphicsPipeline* curr_pipeline = curr_material->pipeline().get();


		//matrices
		glm::mat4 model = m_scene->getModels()[i].modelMatrix();

		
		//glm::mat4 model = scene->getModels()[i].modelMatrix();
		//model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));


		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(frame_counter * 0.01f), glm::vec3(0., 1, 0));
		glm::vec3 trans_vec = glm::vec3{ model[3][0], model[3][1], model[3][2] };
		glm::mat4 trans_origin = glm::translate(glm::mat4(1.0f), -trans_vec);
		glm::mat4 trans_back = glm::translate(glm::mat4(1.0f), trans_vec);

		glm::mat4 modelmat = trans_back * rot * trans_origin * model;
		push_constant.mat1 = modelmat;


		push_constant.mat2 = glm::inverseTranspose(glm::mat3(m_scene->getCamera()->viewMatrix() * modelmat));
		

		


		//Write directional lights to buffer
		
		//Create Global descriptor set for that frame if it doenst exists aready
		if (m_global_descriptor[frame_num].descriptorSet() == nullptr) 
		{
			m_global_descriptor[frame_num].setDescriptorSetLayout(0, curr_pipeline);

			m_global_descriptor[frame_num].bindUniformBuffer(0, m_scene_info_buffer.get(), sizeof(SceneInfo));
			m_global_descriptor[frame_num].bindUniformBuffer(1, m_camera_buffer.get(), sizeof(CameraData));
			m_global_descriptor[frame_num].bindUniformBuffer(2, m_dir_lights_buffer.get(), sizeof(GPUDirLight) * MAX_DIR_LIGHTS);
			m_global_descriptor[frame_num].bindStorageBuffer(3, m_point_lights_buffer.get(), sizeof(GPUPointLight) * MAX_POINT_LIGHTS);

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
			
			//m_render_backend->RC_bindDescriptorSet(curr_material->pipeline(), m_global_descriptor[frame_num], offset_count, offset);
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
		Material* curr_material = m_scene->skybox()->material().get();
		Mesh* curr_mesh = m_scene->skybox()->mesh().get();
		
		cmd_buffer.bindMaterial(*curr_material);

		MatrixPushConstant push_constant;
		push_constant.mat1 = glm::mat3(m_scene->getCamera()->viewMatrix()); //view without tranlation
		push_constant.mat2 = m_scene->getCamera()->projectionMatrix();	//projection
		cmd_buffer.setPushConstants(push_constant);
		
		cmd_buffer.bindVertexBuffer(*curr_mesh->getVertexBuffer());
		cmd_buffer.bindIndexBuffer(*curr_mesh->getIndexBuffer());

		cmd_buffer.drawIndexed(curr_mesh->getIndexBuffer()->getIndexCount());
	}




}