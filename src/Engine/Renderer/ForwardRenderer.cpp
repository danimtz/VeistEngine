#include "ForwardRenderer.h"

#include <GLFW/glfw3.h>

#include "Engine/Scenes/Scene.h"
#include "Engine/Renderer/RenderModule.h"





void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	std::unique_ptr<ShaderBuffer> test_buff = std::make_unique<ShaderBuffer>(10,2, ShaderBufferUsage::Uniform); //test buffer suballocations

	
	

	test_buff->setData("1234567890", 10, 0);
	test_buff->setData("11681", 3, 1);



	m_camera_buffer = std::make_unique<ShaderBuffer>(sizeof(CameraData), m_render_backend->getSwapchainBufferCount(), ShaderBufferUsage::Uniform);
	m_scene_info_buffer = std::make_unique<ShaderBuffer>(sizeof(SceneInfo), m_render_backend->getSwapchainBufferCount(), ShaderBufferUsage::Uniform);
	m_dir_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUDirLight) * MAX_DIR_LIGHTS, m_render_backend->getSwapchainBufferCount(), ShaderBufferUsage::Uniform);
	m_point_lights_buffer = std::make_unique<ShaderBuffer>(sizeof(GPUPointLight) * MAX_POINT_LIGHTS, m_render_backend->getSwapchainBufferCount(), ShaderBufferUsage::Storage);

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

	
	Material* last_material = nullptr;
	GraphicsPipeline* last_pipeline = nullptr;


	//Write per-frame buffers (camera, lights)
	m_camera_data.projection = scene->getCamera()->projectionMatrix();
	m_camera_data.view = scene->getCamera()->viewMatrix();
	m_camera_data.view_projection = scene->getCamera()->viewProjectionMatrix();
	m_camera_buffer.get()->setData(&m_camera_data, sizeof(m_camera_data), frame_num);

	m_scene_info.dir_lights_count = scene->getDirLights().size();
	m_scene_info.point_lights_count = scene->getPointLights().size();
	m_scene_info_buffer.get()->setData(&m_scene_info, sizeof(m_scene_info), frame_num);


	//Directional lighting
	if (m_scene_info.dir_lights_count > 0) {
		GPUDirLight dir_lights[MAX_DIR_LIGHTS];
		memcpy(dir_lights, scene->getDirLights().data(), sizeof(DirectionalLight)* m_scene_info.dir_lights_count);
		glm::mat4 world_to_view = glm::mat4(glm::mat3(scene->getCamera()->viewMatrix()));
		for (int i = 0; i < m_scene_info.dir_lights_count; i++) {
			dir_lights[i].direction = world_to_view * glm::vec4(scene->getDirLights()[i].direction(), 1.0);
		}
		m_dir_lights_buffer.get()->setData(&dir_lights, sizeof(GPUDirLight)* MAX_DIR_LIGHTS, frame_num);
	}


	//Point lights
	if (m_scene_info.point_lights_count > 0){
		GPUPointLight point_lights[MAX_POINT_LIGHTS];
		memcpy(point_lights, scene->getPointLights().data(), sizeof(PointLight)* m_scene_info.point_lights_count);
		scene->getCamera()->viewMatrix();
		for (int i = 0; i < m_scene_info.point_lights_count; i++) {
			point_lights[i].position = scene->getCamera()->viewMatrix() * glm::vec4(scene->getPointLights()[i].position(), 1.0);
		}
		m_point_lights_buffer.get()->setData(&point_lights, sizeof(GPUDirLight) * MAX_POINT_LIGHTS, frame_num);
	}



	//Render each model
	for (int i = 0; i < scene->getModels().size(); i++) {
		
		Mesh* curr_mesh = scene->getModels()[i].mesh().get();
		Material* curr_material = scene->getModels()[i].material().get();
		GraphicsPipeline* curr_pipeline = curr_material->materialData().pipeline().get();


		//matrices
		glm::mat4 model = scene->getModels()[i].modelMatrix();

		
		//glm::mat4 model = scene->getModels()[i].modelMatrix();
		//model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));


		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), glm::radians(frame_counter * 0.1f), glm::vec3(0., 1, 0));
		glm::vec3 trans_vec = glm::vec3{ model[3][0], model[3][1], model[3][2] };
		glm::mat4 trans_origin = glm::translate(glm::mat4(1.0f), -trans_vec);
		glm::mat4 trans_back = glm::translate(glm::mat4(1.0f), trans_vec);

		glm::mat4 modelmat = trans_back * rot * trans_origin * model;
		push_constant.model_mat = modelmat;


		push_constant.normal_mat = glm::inverseTranspose(glm::mat3(scene->getCamera()->viewMatrix() * modelmat));
		

		


		//Write directional lights to buffer
		
		//Create Global descriptor set for that frame if it doenst exists aready
		if (m_global_descriptor[frame_num].descriptorSet() == nullptr) {
			m_global_descriptor[frame_num].setDescriptorSetLayout(0, curr_pipeline);

			m_global_descriptor[frame_num].bindUniformBuffer(0, m_scene_info_buffer.get(), sizeof(SceneInfo));
			m_global_descriptor[frame_num].bindUniformBuffer(1, m_camera_buffer.get(), sizeof(CameraData));
			m_global_descriptor[frame_num].bindUniformBuffer(2, m_dir_lights_buffer.get(), sizeof(GPUDirLight) * MAX_DIR_LIGHTS);
			m_global_descriptor[frame_num].bindStorageBuffer(3, m_point_lights_buffer.get(), sizeof(GPUPointLight) * MAX_POINT_LIGHTS);

			m_global_descriptor[frame_num].buildDescriptorSet();
		}


		//Check if pipeline changed
		if (curr_pipeline != last_pipeline) {
			

			m_render_backend->RC_bindGraphicsPipeline(curr_material->materialData().pipeline());
			m_render_backend->RC_pushConstants(curr_material->materialData().pipeline(), push_constant);

			constexpr int offset_count = 4;
			uint32_t offset[offset_count];
			offset[0] = m_scene_info_buffer.get()->offset() * frame_num;
			offset[1] = m_camera_buffer.get()->offset() * frame_num;
			offset[2] = m_dir_lights_buffer.get()->offset() * frame_num;
			offset[3] = m_point_lights_buffer.get()->offset() * frame_num;
			m_render_backend->RC_bindDescriptorSet(curr_material->materialData().pipeline(), m_global_descriptor[frame_num], offset_count, offset);

			//Check if material changed
			if (curr_material != last_material) {

				//Change material descriptors
				if (curr_material->materialData().textures().size() > 0) {
					//texture descriptor
					m_render_backend->RC_bindDescriptorSet(curr_material->materialData().pipeline(), curr_material->descriptorSet(), 0, nullptr);
				}

			}
		}
		

		m_render_backend->RC_bindVertexBuffer(curr_mesh->getVertexBuffer());
		m_render_backend->RC_bindIndexBuffer(curr_mesh->getIndexBuffer());

		m_render_backend->RC_drawIndexed(curr_mesh->getIndexBuffer()->getSize());
	}

}