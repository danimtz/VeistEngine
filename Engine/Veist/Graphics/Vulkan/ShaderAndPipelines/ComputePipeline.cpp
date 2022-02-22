#include "pch.h"
#include "ComputePipeline.h"

#include "Veist/Graphics/RenderModule.h"


namespace Veist
{


ComputePipeline::ComputePipeline(const std::string& shader_name)
{

	createShaderProgram(shader_name);
	createPipelineLayout();
	createPipeline();

}



void ComputePipeline::createShaderProgram(const std::string& shader_name)
{

	m_shader_program = ShaderProgram::Create(shader_name, true);


}


void ComputePipeline::createPipelineLayout()
{

	VkPipelineLayoutCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	create_info.pNext = nullptr;

	create_info.pushConstantRangeCount = m_shader_program->pushConstantRanges().size();
	if (create_info.pushConstantRangeCount != 0)
	{
		create_info.pPushConstantRanges = &m_shader_program->pushConstantRanges().at(0);
	}
	create_info.flags = 0;
	create_info.setLayoutCount = m_shader_program->descriptorLayouts().size();
	create_info.pSetLayouts = m_shader_program->descriptorLayouts().data();


	VkDevice device = RenderModule::getBackend()->getDevice();
	
	VK_CHECK(vkCreatePipelineLayout(device, &create_info, nullptr, &m_pipeline_layout));

	VkPipelineLayout layout = m_pipeline_layout;
	RenderModule::getBackend()->pushToDeletionQueue([device, layout]() { vkDestroyPipelineLayout(device, layout, nullptr); });

}


void ComputePipeline::createPipeline()
{
	VkDevice device = RenderModule::getBackend()->getDevice();

	VkComputePipelineCreateInfo compute_create_info = {};
	compute_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	compute_create_info.pNext = VK_NULL_HANDLE;

	compute_create_info.stage = m_shader_program->pipelineStages()[0];
	compute_create_info.layout = m_pipeline_layout;
	
	compute_create_info.flags = 0;
	compute_create_info.basePipelineHandle = VK_NULL_HANDLE;
	compute_create_info.basePipelineIndex = -1;
	



	vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &compute_create_info, nullptr, &m_pipeline );




	VkPipeline pipeline = m_pipeline;
	RenderModule::getBackend()->pushToDeletionQueue([device, pipeline]() { //TODO: if pipeline gets rebuilt this should be destroyed
		vkDestroyPipeline(device, pipeline, nullptr);
	});

}

}