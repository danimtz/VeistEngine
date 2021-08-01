#include "VulkanShader.h"
#include "spirv_reflect.hpp"



std::shared_ptr<VulkanShaderProgram> VulkanShaderProgram::Create(std::string shader_name, std::string folder_path)
{

	//Check if shader with that name exists, if it does return pointer to it
			//TODO

	//Else create new shader
	return std::make_shared<VulkanShaderProgram>(shader_name, folder_path);

}



VulkanShaderProgram::VulkanShaderProgram(std::string shader_name, std::string folder_path)
{

	//Simple shaders only for now. Maybe this should be a loop over all shader stages
	std::string filepath_vert = folder_path + shader_name + ".vert.spv";
	std::string filepath_frag = folder_path + shader_name + ".frag.spv";

	createShaderModule(filepath_vert.c_str(), VulkanShaderType::Vertex);
	CONSOLE_LOG("Loaded vertex shader");
	createShaderModule(filepath_frag.c_str(), VulkanShaderType::Fragment);
	CONSOLE_LOG("Loaded fragment shader");


	reflectShader();

}


void VulkanShaderProgram::createShaderModule(const char* file_path, VulkanShaderType shader_type)
{


	//open file with cursor at the end
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		CRITICAL_ERROR_LOG("Could not open shader file");
	}

	//Get file size by inspecting location of cursor(since it is at the end of the file it gives size in bytes)
	size_t file_size = (size_t)file.tellg();

	//spirv expects the buffer to be on uint32. reserve enough for entire file
	std::vector<uint32_t> buffer(file_size / sizeof(uint32_t));

	//put file cursor at start
	file.seekg(0);

	//load file into buffer
	file.read((char*)buffer.data(), file_size);
	file.close();


	//Create vulkan shader module
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.pNext = nullptr;

	//codeSize has to be in bytes
	create_info.codeSize = buffer.size() * sizeof(uint32_t);
	create_info.pCode = buffer.data();


	VkShaderModule shader_module;
	VkDevice device = static_cast<VkDevice>(RenderModule::getRenderBackend()->getDevice());
	VK_CHECK(vkCreateShaderModule(device, &create_info, nullptr, &shader_module));
	RenderModule::getRenderBackend()->pushToDeletionQueue([=]() {vkDestroyShaderModule(device, shader_module, nullptr); });
	m_shader_module.push_back(shader_module);


	//Create shader stage
	VkPipelineShaderStageCreateInfo stage_create_info = {};
	stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_create_info.pNext = nullptr;
	stage_create_info.stage = static_cast<VkShaderStageFlagBits>(shader_type);

	//shader module
	stage_create_info.module = shader_module;

	//shader entry point
	stage_create_info.pName = "main";

	m_pipeline_stages.push_back(stage_create_info);


}



void VulkanShaderProgram::reflectShader() 
{




}