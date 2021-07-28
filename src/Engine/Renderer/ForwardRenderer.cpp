#include "ForwardRenderer.h"


#include <GLFW/glfw3.h>



void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
}

ForwardRenderer::~ForwardRenderer()
{

}


void ForwardRenderer::renderScene(Scene* scene)
{


	//THIS WILL BE REFACTORED INTO MANY THINGS-> graphics context instead of backend, renderpass and cmd class maybe instead of RC_begin frame
	//Binding stuff should be abstracted out of the monolithic VulkanRenderBackend at some point as well.

	m_render_backend->RC_beginFrame();

	uint32_t framenum = m_render_backend->getFrameNumber();
	MatrixPushConstant push_constant;

	//curentaly sharing model matrix
	glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(framenum * 0.03f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));
	push_constant.MVPmatrix = scene->getCamera()->viewProjectionMatrix() * model;
	push_constant.Nmatrix = glm::inverseTranspose(glm::mat3(scene->getCamera()->viewMatrix() * model));

	
	for (int i = 0; i < scene->getModels().size(); i++) {

		Mesh curr_mesh = *scene->getModels()[i].mesh().get();
		Material curr_material = *scene->getModels()[i].material().get();
		//m_render_backend->RC_bindGraphicsPipeline( m_pipeline );
		m_render_backend->RC_bindGraphicsPipeline(curr_material.pipeline());

		
		//m_render_backend->RC_pushConstants(m_pipeline, push_constant);
		m_render_backend->RC_pushConstants(curr_material.pipeline(), push_constant);

		//m_render_backend->RC_bindVertexBuffer(test_mesh.getVertexBuffer());
		m_render_backend->RC_bindVertexBuffer(curr_mesh.getVertexBuffer());
		m_render_backend->RC_bindIndexBuffer(curr_mesh.getIndexBuffer());

		//m_render_backend->RC_drawSumbit(tt_mesh.getVertexBuffer()->getSize());
		m_render_backend->RC_drawIndexed(curr_mesh.getIndexBuffer()->getSize());


	}
	
	m_render_backend->RC_endFrame();

	//VkDevice device = static_cast<VkDevice>(m_render_backend->getDevice());

}