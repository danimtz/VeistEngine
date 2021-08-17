#include "ForwardRenderer.h"

#include <GLFW/glfw3.h>

#include "Engine/Scenes/Scene.h"
#include "Engine/Renderer/RenderModule.h"



static constexpr int MAX_DIR_LIGHTS = 4;


void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	std::unique_ptr<UniformBuffer> test_buff = std::make_unique<UniformBuffer>(10,2); //test buffer suballocations

	
	

	test_buff->setData("1234567890", 10, 0);
	test_buff->setData("11681", 3, 1);



	m_camera_buffer = std::make_unique<UniformBuffer>(sizeof(CameraData), m_render_backend->getSwapchainBufferCount());
	m_dir_lights_buffer = std::make_unique<UniformBuffer>(sizeof(GPUDirLight)*MAX_DIR_LIGHTS, m_render_backend->getSwapchainBufferCount());
	m_global_descriptor.resize(m_render_backend->getSwapchainBufferCount());

}

ForwardRenderer::~ForwardRenderer()
{

}


void ForwardRenderer::renderScene(Scene* scene)
{
	//THIS WILL BE REFACTORED INTO MANY THINGS-> graphics context instead of backend, renderpass and cmd class maybe instead of RC_begin frame
	

	uint32_t frame_counter = m_render_backend->getFrameNumber();
	uint32_t frame_num = m_render_backend->getSwapchainImageNumber();
	MatrixPushConstant push_constant;

	//curentaly sharing model matrix
	
	Material* last_material = nullptr;
	GraphicsPipeline* last_pipeline = nullptr;


	//Write per-frame buffers (camera, lights)
	m_camera_data.projection = scene->getCamera()->projectionMatrix();
	m_camera_data.view = scene->getCamera()->viewMatrix();
	m_camera_data.view_projection = scene->getCamera()->viewProjectionMatrix();
	m_camera_buffer.get()->setData(&m_camera_data, sizeof(m_camera_data), frame_num);

	//Directional lighting
	uint32_t dir_lights_count = scene->getDirLights().size();
	GPUDirLight dir_lights[MAX_DIR_LIGHTS];

	glm::mat4 world_to_view = glm::mat4(glm::mat3(scene->getCamera()->viewMatrix()));
	for (int i = 0; i < dir_lights_count; i++) {
		GPUDirLight &light = dir_lights[i];
		memcpy(&light, &scene->getDirLights()[i], sizeof(DirectionalLight));
		light.num_lights = dir_lights_count;
		light.direction = world_to_view * glm::vec4(scene->getDirLights()[i].direction(), 1.0);
	}
	m_dir_lights_buffer.get()->setData(&dir_lights, sizeof(GPUDirLight)* MAX_DIR_LIGHTS, frame_num);


	//Render each model
	for (int i = 0; i < scene->getModels().size(); i++) {
		
		Mesh* curr_mesh = scene->getModels()[i].mesh().get();
		Material* curr_material = scene->getModels()[i].material().get();
		GraphicsPipeline* curr_pipeline = curr_material->pipeline().get();




		glm::mat4 model = glm::rotate(scene->getModels()[i].modelMatrix(), glm::radians(frame_counter * 0.03f), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));
		push_constant.model_mat = model;
		push_constant.normal_mat = glm::inverseTranspose(glm::mat3(scene->getCamera()->viewMatrix() * model));
		

		


		//Write directional lights to buffer
		
		//Create Global descriptor set for that frame if it doenst exists aready
		if (m_global_descriptor[frame_num].descriptorSet() == nullptr) {
			m_global_descriptor[frame_num].setDescriptorSetLayout(0, curr_pipeline);
			m_global_descriptor[frame_num].bindUniformBuffer(0, m_camera_buffer.get(), sizeof(CameraData));
			m_global_descriptor[frame_num].bindUniformBuffer(1, m_dir_lights_buffer.get(), sizeof(GPUDirLight) * MAX_DIR_LIGHTS);
			m_global_descriptor[frame_num].buildDescriptorSet();
		}


		//Check if pipeline changed
		if (curr_pipeline != last_pipeline) {
			

			m_render_backend->RC_bindGraphicsPipeline(curr_material->pipeline());
			m_render_backend->RC_pushConstants(curr_material->pipeline(), push_constant);

			uint32_t offset[2];
			offset[0] = m_camera_buffer.get()->offset() * frame_num;
			offset[1] = m_dir_lights_buffer.get()->offset() * frame_num;
			m_render_backend->RC_bindDescriptorSet(curr_material->pipeline(), m_global_descriptor[frame_num], 2, offset);

			//Check if material changed
			if (curr_material != last_material) {


				//Change material descriptors
				


			}
		}
		

		m_render_backend->RC_bindVertexBuffer(curr_mesh->getVertexBuffer());
		m_render_backend->RC_bindIndexBuffer(curr_mesh->getIndexBuffer());

		m_render_backend->RC_drawIndexed(curr_mesh->getIndexBuffer()->getSize());
	}
}