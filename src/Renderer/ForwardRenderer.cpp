#include "ForwardRenderer.h"




void ForwardRenderer::init(std::shared_ptr<RenderBackend> backend)
{
	setRenderBackend(backend);
	
	VertexDescription vertex_desc = {0, {
		{VertexAttributeType::Float3, "position"},
		{VertexAttributeType::Float3, "normal"},
		{VertexAttributeType::Float3, "color"} }};

	m_pipeline = GraphicsPipeline::Create("triangle", "..\\..\\src\\Shaders\\", vertex_desc);

	
}

ForwardRenderer::~ForwardRenderer()
{

}

void ForwardRenderer::renderScene()
{

	m_render_backend->RC_beginFrame();

	
	m_render_backend->RC_bindGraphicsPipeline( m_pipeline );

	m_render_backend->RC_bindVertexBuffer(test_mesh.getVertexBuffer());

	m_render_backend->RC_drawSumbit(test_mesh.getVertexBuffer()->getSize());

	m_render_backend->RC_endFrame();

	//VkDevice device = static_cast<VkDevice>(m_render_backend->getDevice());

}