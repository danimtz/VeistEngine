

#include "Engine/Renderer/Vulkan/ShaderAndPipelines/GraphicsPipeline.h"
#include "Engine/Renderer/RenderModule.h"


GraphicsPipeline::GraphicsPipeline(std::string shader_name, const VertexDescription& vertex_desc,  VkPrimitiveTopology topology,
	VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, DepthTest depth_test) 
{
	GraphicsPipelineBuilder pipeline_builder = {shader_name, vertex_desc, topology, polygon_mode, cull_mode, front_face, depth_test };

	m_pipeline = pipeline_builder.m_pipeline;
	m_pipeline_layout = pipeline_builder.m_pipeline_layout;
	m_shader_program = pipeline_builder.m_shader_program;

};



GraphicsPipelineBuilder::GraphicsPipelineBuilder(std::string shader_name, const VertexDescription&vertex_desc, VkPrimitiveTopology topology,
	VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, DepthTest depth_test) :
	m_topology(topology), 
	m_polygon_mode(polygon_mode), 
	m_cull_mode(cull_mode), 
	m_front_face(front_face),
	m_depth_test(depth_test)
{
	
	


	

	createShaderProgram(shader_name);

	setVertexInputDescriptions(vertex_desc);

	createPipelineStates();
	createPipelineLayout();
	createPipeline();
	
}


void GraphicsPipelineBuilder::createShaderProgram(std::string shader_name)
{

	m_shader_program = ShaderProgram::Create(shader_name);
	
}


void GraphicsPipelineBuilder::setVertexInputDescriptions(const VertexDescription& vertex_desc)
{
	//Parse Vertexdescription into vulkan vertex attribute and vertex binding descriptors
	uint32_t location_count = 0;
	
	VkVertexInputBindingDescription binding_desc = {};
	binding_desc.binding = 0;//vertex_desc.getBinding();
	binding_desc.stride = vertex_desc.getStride();
	binding_desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
	m_vertex_bindings.push_back(binding_desc);

	auto attributes = vertex_desc.getVertexAttributes();
	for (uint32_t j = 0; j < attributes.size(); j++) {

		VkVertexInputAttributeDescription attribute_desc = {};
		attribute_desc.binding = 0;//vertex_desc.getBinding();
		attribute_desc.location = location_count;
		location_count++;
		attribute_desc.offset = attributes[j].m_offset;

		switch (attributes[j].m_type) {
		case VertexAttributeType::Float:
			attribute_desc.format = VK_FORMAT_R32_SFLOAT;
			break;
		case VertexAttributeType::Float2:
			attribute_desc.format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case VertexAttributeType::Float3:
			attribute_desc.format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case VertexAttributeType::Float4:
			attribute_desc.format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		//More can be added later
		default:
			CRITICAL_ERROR_LOG("Vertex Attribute Type invalid on Pipeline creation");
		}

		m_vertex_attributes.push_back(attribute_desc);
	}
	
	


}





void GraphicsPipelineBuilder::createPipelineStates()
{
	////////////////////////////////////////
	//Vertex input state (nothing for now)//
	////////////////////////////////////////
	m_vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	m_vertex_input_info.pNext = nullptr;


	m_vertex_input_info.vertexBindingDescriptionCount = m_vertex_bindings.size();
	m_vertex_input_info.pVertexBindingDescriptions = m_vertex_bindings.data();

	m_vertex_input_info.vertexAttributeDescriptionCount = m_vertex_attributes.size();
	m_vertex_input_info.pVertexAttributeDescriptions = m_vertex_attributes.data();



	/////////////////////////
	//Input assembly state //
	/////////////////////////
	m_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	m_input_assembly_info.pNext = nullptr;
	m_input_assembly_info.topology = m_topology;
	m_input_assembly_info.primitiveRestartEnable = VK_FALSE;


	////////////////////////
	//Rasterization state //
	////////////////////////
	m_rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	m_rasterizer_info.pNext = nullptr;
	m_rasterizer_info.depthClampEnable = VK_FALSE;
	m_rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
	m_rasterizer_info.polygonMode = m_polygon_mode;
	m_rasterizer_info.lineWidth = 1.0f;
	m_rasterizer_info.cullMode = m_cull_mode;
	m_rasterizer_info.frontFace = m_front_face;
	//no depth bias
	m_rasterizer_info.depthBiasEnable = VK_FALSE;
	m_rasterizer_info.depthBiasConstantFactor = 0.0f;
	m_rasterizer_info.depthBiasClamp = 0.0f;
	m_rasterizer_info.depthBiasSlopeFactor = 0.0f;


	////////////////////////
	//MultiSampling state //
	////////////////////////
	m_multisample_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	m_multisample_state_info.pNext = nullptr;

	m_multisample_state_info.sampleShadingEnable = VK_FALSE;

	m_multisample_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //multisampled ? device->GetMsaaSamples() : VK_SAMPLE_COUNT_1_BIT; none for now
	m_multisample_state_info.minSampleShading = 1.0f;
	m_multisample_state_info.pSampleMask = nullptr;
	m_multisample_state_info.alphaToCoverageEnable = VK_FALSE;
	m_multisample_state_info.alphaToOneEnable = VK_FALSE;


	/////////////////////////
	//Color blending state //
	/////////////////////////
	m_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	m_color_blend_attachment_state.blendEnable = VK_TRUE;
	//Alpha blending
	m_color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	m_color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	m_color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD; 
	m_color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
	m_color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
	m_color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD; 

	m_color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	m_color_blend_state_info.logicOpEnable = VK_FALSE;
	m_color_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
	m_color_blend_state_info.attachmentCount = 1; //Only 1 attachment for now
	m_color_blend_state_info.pAttachments = &m_color_blend_attachment_state;
	m_color_blend_state_info.blendConstants[0] = 0.0f; 
	m_color_blend_state_info.blendConstants[1] = 0.0f; 
	m_color_blend_state_info.blendConstants[2] = 0.0f; 
	m_color_blend_state_info.blendConstants[3] = 0.0f; 



	/////////////////////////
	//Depth stencil state  //
	/////////////////////////

	m_depth_stencil_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	m_depth_stencil_state_info.pNext = nullptr;


	m_depth_stencil_state_info.depthCompareOp = VK_COMPARE_OP_LESS; //VK_COMPARE_OP_LESS
	m_depth_stencil_state_info.depthBoundsTestEnable = VK_FALSE;
	//m_depth_stencil_state_info.minDepthBounds = 0.0f; //Optional
	//m_depth_stencil_state_info.maxDepthBounds = 1.0f; //Optional
	m_depth_stencil_state_info.stencilTestEnable = VK_FALSE;


	switch (m_depth_test) {
		case DepthTest::None:
			m_depth_stencil_state_info.depthTestEnable = VK_FALSE;
			m_depth_stencil_state_info.depthWriteEnable = VK_FALSE;
			break;
		case DepthTest::Read:
			m_depth_stencil_state_info.depthTestEnable = VK_TRUE;
			m_depth_stencil_state_info.depthWriteEnable = VK_FALSE;
			break;
		case DepthTest::Write:
			m_depth_stencil_state_info.depthTestEnable = VK_FALSE;
			m_depth_stencil_state_info.depthWriteEnable = VK_TRUE;
			break;
		case DepthTest::ReadWrite:
			m_depth_stencil_state_info.depthTestEnable = VK_TRUE;
			m_depth_stencil_state_info.depthWriteEnable = VK_TRUE;
			break;
	}

	/////////////////////////
	//Viewport state	   //
	/////////////////////////

	VkExtent2D* swapchain_extent = RenderModule::getRenderBackend()->getSwapchainExtent();

	m_viewport.x = 0.0f;
	m_viewport.y = 0.0f;
	m_viewport.width = (float)swapchain_extent->width;
	m_viewport.height = (float)swapchain_extent->height;
	m_viewport.minDepth = 0.0f;
	m_viewport.maxDepth = 1.0f;

	m_scissor.offset = { 0, 0 };
	m_scissor.extent = *swapchain_extent;

	m_viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	m_viewport_state_info.pNext = nullptr;

	m_viewport_state_info.viewportCount = 1;
	m_viewport_state_info.pViewports = &m_viewport;
	m_viewport_state_info.scissorCount = 1;
	m_viewport_state_info.pScissors = &m_scissor;
}


void GraphicsPipelineBuilder::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	create_info.pNext = nullptr;

	//Add pushconstant from MatrixPushConstant struct in graphicspipeline.h TEMPORARY, WILL NEED REFACTORING LATER ON
	//setup push constants
	/*VkPushConstantRange push_constant;
	push_constant.offset = 0;
	push_constant.size = sizeof(MatrixPushConstant);
	push_constant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	*/
	create_info.pushConstantRangeCount = 1;
	create_info.pPushConstantRanges = &m_shader_program->pushConstantRanges().at(0);

	create_info.flags = 0;
	create_info.setLayoutCount = m_shader_program->descriptorLayouts().size();
	create_info.pSetLayouts = m_shader_program->descriptorLayouts().data();
	
	
	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VkPipelineLayout layout;
	VK_CHECK(vkCreatePipelineLayout(device, &create_info, nullptr, &layout));
	m_pipeline_layout = layout;
	RenderModule::getRenderBackend()->pushToDeletionQueue([device, layout]() {vkDestroyPipelineLayout(device, layout, nullptr); });
}


void GraphicsPipelineBuilder::createPipeline()
{
	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	VkRenderPass render_pass = RenderModule::getRenderBackend()->getRenderPass();

	VkGraphicsPipelineCreateInfo pipeline_info = {};
	pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_info.pNext = nullptr;
	
	pipeline_info.stageCount = m_shader_program->shaderModules().size();
	pipeline_info.pStages = m_shader_program->pipelineStages().data();
	pipeline_info.pVertexInputState = &m_vertex_input_info;
	pipeline_info.pInputAssemblyState = &m_input_assembly_info;
	pipeline_info.pViewportState = &m_viewport_state_info;
	pipeline_info.pRasterizationState = &m_rasterizer_info;
	pipeline_info.pMultisampleState = &m_multisample_state_info;
	pipeline_info.pColorBlendState = &m_color_blend_state_info;
	pipeline_info.pDepthStencilState = &m_depth_stencil_state_info;
	pipeline_info.layout = m_pipeline_layout;
	pipeline_info.renderPass = render_pass;
	pipeline_info.subpass = 0; //This needs to be an argument later if subpasses are used for deferred rendering. NOTE TO FUTURE ME
	pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

	//pipeline cahce is VK_NULL_HANDLE for now
	VkPipeline pipeline;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS) {
		CONSOLE_LOG("Pipeline creation FAILED!");
	}
	m_pipeline = pipeline; 
	RenderModule::getRenderBackend()->pushToDeletionQueue(	[device, pipeline](){
		vkDestroyPipeline(device, pipeline, nullptr); 
	});
}