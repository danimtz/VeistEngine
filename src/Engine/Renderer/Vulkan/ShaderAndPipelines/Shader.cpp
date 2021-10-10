#include "Engine/Renderer/Vulkan/ShaderAndPipelines/Shader.h"

#include "spirv_cross.hpp"

#include "Engine/Renderer/RenderModule.h"

#include <shaderc/shaderc.hpp>


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

std::shared_ptr<ShaderProgram> ShaderProgram::Create(std::string shader_name, std::string folder_path)
{

	//Check if shader with that name exists, if it does return pointer to it
			//TODO

	//Else create new shader
	return std::make_shared<ShaderProgram>(shader_name, folder_path);

}



ShaderProgram::ShaderProgram(std::string shader_name, std::string folder_path)
{

	//Simple shaders only for now. Maybe this should be a loop over all shader stages
	std::string filepath_vert = folder_path + shader_name + ".vert";
	std::string filepath_frag = folder_path + shader_name + ".frag";

	createShaderModule(filepath_vert.c_str(), ShaderStageFlag::Vertex);
	CONSOLE_LOG("Loaded vertex shader");
	createShaderModule(filepath_frag.c_str(), ShaderStageFlag::Fragment);
	CONSOLE_LOG("Loaded fragment shader");


	createDescriptorSetLayouts();

}


void ShaderProgram::createShaderModule(const char* file_path, ShaderStageFlag shader_type)
{

/*
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

	*/
	std::ifstream in(file_path, std::ios::in | std::ios::binary);

	std::string raw_glsl;
	if (in)
	{
		in.seekg(0, std::ios::end);
		raw_glsl.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&raw_glsl[0], raw_glsl.size());
		in.close();
	}
	else
	{
		CRITICAL_ERROR_LOG("Error reading shaders. Cannot read shader file");
	}



	shaderc_shader_kind shaderc_kind;

	switch (shader_type)
	{
		case ShaderStageFlag::Vertex:
			shaderc_kind = shaderc_shader_kind::shaderc_vertex_shader;
			break;

		case ShaderStageFlag::Fragment:
			shaderc_kind = shaderc_shader_kind::shaderc_fragment_shader;
			break;

		case ShaderStageFlag::Compute:
			shaderc_kind = shaderc_shader_kind::shaderc_compute_shader;
			break;

		case ShaderStageFlag::Geometry:
			shaderc_kind = shaderc_shader_kind::shaderc_geometry_shader;
			break;
	}


	shaderc::Compiler compiler;
	shaderc::CompileOptions options;
	shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(raw_glsl, shaderc_kind, file_path);
	if (result.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		//handle errors
		CRITICAL_ERROR_LOG("Error compiling shaders");
	}
	std::vector<uint32_t> shader_spv;
	shader_spv.assign(result.cbegin(), result.cend());



	//Create vulkan shader module
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.pNext = nullptr;

	//codeSize has to be in bytes
	create_info.codeSize = shader_spv.size() * sizeof(uint32_t);
	create_info.pCode = shader_spv.data();


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



	reflectShaderModule(shader_spv, shader_type); //on reflect shader extract all sets and bindings etc. 
	//Then make a function clled build descriptor set layouts or something that sorts them and creates them and the pipeline layouts2



}



void ShaderProgram::reflectShaderModule(std::vector<uint32_t>& buffer, ShaderStageFlag shader_type)
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
			auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<VkShaderStageFlagBits>(shader_type));
			m_bindings[set].insert({binding, decriptor_binding});
		} 
		else
		{
			//Add stage flag
			addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(shader_type)); //UNTESTED MIGHT NOT WORK
			
		}

		printf("Uniform Buffer %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);
	}

	//Storage Buffers
	for (auto& resource : resources.storage_buffers)
	{
		unsigned set = comp.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = comp.get_decoration(resource.id, spv::DecorationBinding);
		if (m_bindings[set].find(binding) == m_bindings[set].end()) //if not found
		{
			auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, static_cast<VkShaderStageFlagBits>(shader_type));
			m_bindings[set].insert({ binding, decriptor_binding });
		}
		else
		{
			//Add stage flag
			addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(shader_type)); //UNTESTED MIGHT NOT WORK

		}

		printf("Storage Buffer %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);

	}


	//Combined Image Samplers
	for (auto& resource : resources.sampled_images)
	{
		unsigned set = comp.get_decoration(resource.id, spv::DecorationDescriptorSet);
		unsigned binding = comp.get_decoration(resource.id, spv::DecorationBinding);
		if (m_bindings[set].find(binding) == m_bindings[set].end()) //if not found
		{
			auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<VkShaderStageFlagBits>(shader_type));
			m_bindings[set].insert({ binding, decriptor_binding });
		}
		else
		{
			//Add stage flag
			addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(shader_type)); //UNTESTED MIGHT NOT WORK

		}

		printf("Combined Image Sampler %s at set = %u, binding = %u\n", resource.name.c_str(), set, binding);

	}

	
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




void ShaderProgram::createDescriptorSetLayouts()
{
	
	for (int i = 0; i < MAX_DESCRIPTOR_SETS; i++)
	{
		//Check that descriptor set exists
		if(m_bindings[i].size() == 0)
		{
			continue;
		};

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

		
		VkDevice device = RenderModule::getRenderBackend()->getDevice();

	
		DescriptorSetLayoutCache* layout_cache = RenderModule::getRenderBackend()->getDescriptorAllocator()->layoutCache();
		VkDescriptorSetLayout layout = layout_cache->createDescriptorSetLayout(device, &set_layout_info);

		
		

		m_descriptor_layouts.push_back(layout);
	}

	CONSOLE_LOG("Created Descriptor set layouts")

}