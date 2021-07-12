#include "ForwardRenderer.h"


void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	m_pipeline = new VulkanGraphicsPipeline("triangle", "..\\..\\src\\Shaders\\"); 

	//NOTE TO ME: PIPELINE LAYOUT, SHADER MODULES AND PIPELINE VKOBJEXTS NEED TO BE vkDestroyed BEFORE RENDER BACKEND DOES DESTRUCTION
}

ForwardRenderer::~ForwardRenderer()
{
	delete m_pipeline;
}

void ForwardRenderer::renderScene()
{

	m_render_backend->RC_beginFrame();

	
	m_render_backend->RC_bindGraphicsPipeline( m_pipeline );

	m_render_backend->RC_drawSumbit();

	m_render_backend->RC_endFrame();

	//VkDevice device = static_cast<VkDevice>(m_render_backend->getDevice());

}