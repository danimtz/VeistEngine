#include "ForwardRenderer.h"

#include <GLFW/glfw3.h>

#include "Engine/Scenes/Scene.h"
#include "Engine/Renderer/RenderModule.h"

void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	std::unique_ptr<UniformBuffer> test_buff = std::make_unique<UniformBuffer>(10,2); //test buffer suballocations

	
	

	test_buff->setData("1234567890", 10, 0);
	test_buff->setData("11681", 3, 1);



	m_global_uniform = std::make_unique<UniformBuffer>(sizeof(CameraData), m_render_backend->getSwapchainBufferCount());
	m_global_descriptor.resize(m_render_backend->getSwapchainBufferCount());

}

ForwardRenderer::~ForwardRenderer()
{

}


void ForwardRenderer::renderScene(Scene* scene)
{
	//THIS WILL BE REFACTORED INTO MANY THINGS-> graphics context instead of backend, renderpass and cmd class maybe instead of RC_begin frame
	

	uint32_t frame_counter = m_render_backend->getFrameNumber();
	MatrixPushConstant push_constant;

	//curentaly sharing model matrix
	
	Material* last_material = nullptr;
	GraphicsPipeline* last_pipeline = nullptr;

	for (int i = 0; i < scene->getModels().size(); i++) {
		uint32_t frame_num = m_render_backend->getSwapchainImageNumber();
		Mesh* curr_mesh = scene->getModels()[i].mesh().get();
		Material* curr_material = scene->getModels()[i].material().get();
		GraphicsPipeline* curr_pipeline = curr_material->pipeline().get();




		glm::mat4 model = glm::rotate(scene->getModels()[i].modelMatrix(), glm::radians(frame_counter * 0.03f), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));
		push_constant.model_mat = model;
		

		//Write camera data to buffer
		m_camera_data.projection = scene->getCamera()->projectionMatrix();
		m_camera_data.view = scene->getCamera()->viewMatrix();
		m_camera_data.view_projection = scene->getCamera()->viewProjectionMatrix();
		m_camera_data.normal_matrix = glm::inverseTranspose(glm::mat3(scene->getCamera()->viewMatrix() * model));
		m_global_uniform.get()->setData(&m_camera_data, sizeof(m_camera_data), frame_num);

		
		//Create Global descriptor set for that frame if it doenst exists aready
		if (m_global_descriptor[frame_num].descriptorSet() == nullptr) {

			m_global_descriptor[frame_num].setDescriptorSetLayout(0, curr_pipeline);
			m_global_descriptor[frame_num].bindUniformBuffer(0, m_global_uniform.get(), sizeof(CameraData));
			m_global_descriptor[frame_num].buildDescriptorSet();
		}





		//Check if pipeline changed
		if (curr_pipeline != last_pipeline) {
			

			m_render_backend->RC_bindGraphicsPipeline(curr_material->pipeline());
			m_render_backend->RC_pushConstants(curr_material->pipeline(), push_constant);

			uint32_t offset = m_global_uniform.get()->offset() * frame_num;
			m_render_backend->RC_bindDescriptorSet(curr_material->pipeline(), m_global_descriptor[frame_num], m_render_backend->getSwapchainBufferCount()-1, &offset);

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