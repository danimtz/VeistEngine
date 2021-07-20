#include "ForwardRenderer.h"




void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	
	


	//I dont like how im using pipelines here. Pipelines are being used as a shader (which is kind of how it works in vulkan)
	//A better way i reckon would be to use shader or material here in the renderer and abstract the usage of the pipeline object away from whoever 
	//is writing the renderer class (which is me in this case but yeah). Then the shader class or material should contain stuff like Pipeline Layout and DescriptorSetLayout
	//and push constant range, VertexDescription(my own thing). Leaving it for now
	m_pipeline = GraphicsPipeline::Create("triangle", "..\\..\\src\\Shaders\\", test_mesh.getVertexBuffer()->getInputDescription()); //

	m_helmetPipeline = GraphicsPipeline::Create("helmet", "..\\..\\src\\Shaders\\", tt_mesh.getVertexBuffer()->getInputDescription()); //
}

ForwardRenderer::~ForwardRenderer()
{

}


void ForwardRenderer::renderScene()
{


	//THIS WILL BE REFACTORED INTO MANY THINGS-> graphics context instead of backend, renderpass and cmd class maybe instead of RC_begin frame
	//Binding stuff should be abstracted out of the monolithic VulkanRenderBackend at some point as well.

	m_render_backend->RC_beginFrame();

	//m_render_backend->RC_bindGraphicsPipeline( m_pipeline );
	m_render_backend->RC_bindGraphicsPipeline(m_helmetPipeline);

	uint32_t framenum = m_render_backend->getFrameNumber();
	
	
	glm::mat4 model = glm::rotate(glm::mat4{ 1.0f }, glm::radians(framenum  * 0.03f), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::degrees(-90.0f), glm::vec3(1, 0, 0));



	glm::vec3 camPos = { 0.f,0.f,-3.f };
	glm::mat4 view = glm::translate(glm::mat4(1.f), camPos); //Camera matrix for looking down z axis is identity matrix

	glm::mat4 projection = glm::perspective(glm::radians(70.f), 800.0f / 600.0f, 0.1f, 200.0f);
	projection[1][1] *= -1; //vulkan flips the y axis

	MatrixPushConstant push_constant;
	push_constant.MVPmatrix = projection * view * model;
	push_constant.Nmatrix = glm::inverseTranspose(glm::mat3(view*model));
	//m_render_backend->RC_pushConstants(m_pipeline, push_constant);
	m_render_backend->RC_pushConstants(m_helmetPipeline, push_constant);

	//m_render_backend->RC_bindVertexBuffer(test_mesh.getVertexBuffer());
	m_render_backend->RC_bindVertexBuffer(tt_mesh.getVertexBuffer());
	m_render_backend->RC_bindIndexBuffer(tt_mesh.getIndexBuffer());

	//m_render_backend->RC_drawSumbit(tt_mesh.getVertexBuffer()->getSize());
	m_render_backend->RC_drawIndexed(tt_mesh.getIndexBuffer()->getSize());
	m_render_backend->RC_endFrame();

	//VkDevice device = static_cast<VkDevice>(m_render_backend->getDevice());

}