#include "Engine/Renderer/Vulkan/ShaderAndPipelines/VulkanShader.h"

#include "spirv_cross.hpp"

#include "Engine/Renderer/RenderModule.h"




static VkDescriptorSetLayoutBinding& getDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlagBits stage_flag)
{
	VkDescriptorSetLayoutBinding layout_binding = {};
	layout_binding.binding = binding;
	layout_binding.descriptorCount = 1;
	layout_binding.descriptorType = descriptor_type;
	layout_binding.stageFlags = stage_flag;
	
	return layout_binding;

}

static void addStageFlagToBinding(VkDescriptorSetLayoutBinding& layout_binding, VkShaderStageFlagBits stage_flag)
{	

	layout_binding.stageFlags = layout_binding.stageFlags | stage_flag;

}

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


	createDescriptorSetLayouts();

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
	VkDevice device = RenderModule::getRenderBackend()->getDevice();
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



	reflectShaderModule(buffer, shader_type); //on reflect shader extract all sets and bindings etc. 
	//Then make a function clled build descriptor set layouts or something that sorts them and creates them and the pipeline layouts2



}



void VulkanShaderProgram::reflectShaderModule(std::vector<uint32_t>& buffer, VulkanShaderType shader_type) 
{

	//Reflection
	spirv_cross::Compiler comp(std::move(buffer));

	// The SPIR-V is now parsed, and we can perform reflection on it.
	spirv_cross::ShaderResources resources = comp.get_shader_resources();

	//Uniform Buffers
	for (auto& resource : resources.uniform_buffers)
	{
		unsigned set = comp.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = comp.get_decoration(resource.id, spv::DecorationBinding);
		if (m_bindings[set].find(binding) == m_bindings[set].end()) //if not found
		{
			auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<VkShaderStageFlagBits>(shader_type));
			m_bindings[set].insert({binding, decriptor_binding});
		} 
		else
		{
			//Add stage flag
			addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(shader_type)); //UNTESTED MIGHT NOT WORK
			
		}

		printf("Buffer %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
	}

	//Storage Buffers

	//Image Samplers
	
	//Push Constants (just one and simple for now)
	if (resources.push_constant_buffers.size() > 1) 
	{
		CRITICAL_ERROR_LOG("Shader creation error: Cannot have more than one push constant!!");
	}
	
	for (auto& resource : resources.push_constant_buffers)
	{
		auto& type = comp.get_type(resource.type_id);

		VkPushConstantRange constant_range = {};
		constant_range.stageFlags = static_cast<VkShaderStageFlagBits>(shader_type);
		constant_range.offset = 0;
		constant_range.size = comp.get_declared_struct_size(type);

		m_push_constants.push_back(constant_range);
	}


	//TODO: subpass attachments, etc etc


	
}




void VulkanShaderProgram::createDescriptorSetLayouts()
{
	
	for (int i = 0; i < MAX_DESCRIPTOR_SETS; i++)
	{
		//Convert bindings to contiguous memory
		std::vector<VkDescriptorSetLayoutBinding> bindings_array;
		bindings_array.reserve(m_bindings[i].size());
		for (int j = 0; j < m_bindings[i].size(); j++) 
		{
			bindings_array.push_back( std::move(m_bindings[i][j]) );
		}

		VkDescriptorSetLayoutCreateInfo set_layout_info = {};

		set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		set_layout_info.bindingCount = bindings_array.size();
		set_layout_info.flags = 0;
		set_layout_info.pBindings = bindings_array.data();
		set_layout_info.pNext = nullptr;

		VkDescriptorSetLayout layout;
		VkDevice device = RenderModule::getRenderBackend()->getDevice();
		VK_CHECK(vkCreateDescriptorSetLayout(device, &set_layout_info, nullptr, &layout));
		RenderModule::getRenderBackend()->pushToDeletionQueue([device, layout]() {vkDestroyDescriptorSetLayout(device, layout, nullptr); });
		

		m_descriptor_layouts.push_back(layout);
	}

	CONSOLE_LOG("Created Descriptor set layouts")

}