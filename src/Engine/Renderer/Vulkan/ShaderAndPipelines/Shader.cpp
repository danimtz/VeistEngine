#include "Engine/Renderer/Vulkan/ShaderAndPipelines/Shader.h"

#include "spirv_cross.hpp"

#include "Engine/Renderer/RenderModule.h"

#include <shaderc/shaderc.hpp>

#include <file_includer.h>

static VkDescriptorSetLayoutBinding getDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type, VkShaderStageFlagBits stage_flag)
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


static std::string getShaderDirectory()
{
	return "..\\..\\src\\Shaders\\";
}


static std::string getShaderCacheDirectory()
{
	return "..\\..\\src\\Shaders\\CachedSpirV\\";
}


static shaderc_shader_kind shaderStageFlagToShaderc(ShaderStageFlag stage)
{
	switch (stage)
	{
	case ShaderStageFlag::Vertex:
		return shaderc_shader_kind::shaderc_vertex_shader;
		break;

	case ShaderStageFlag::Fragment:
		return shaderc_shader_kind::shaderc_fragment_shader;
		break;

	case ShaderStageFlag::Compute:
		return shaderc_shader_kind::shaderc_compute_shader;
		break;

	case ShaderStageFlag::Geometry:
		return shaderc_shader_kind::shaderc_geometry_shader;
		break;

	case ShaderStageFlag::TessC:
		return shaderc_shader_kind::shaderc_tess_control_shader;
		break;

	case ShaderStageFlag::TessE:
		return shaderc_shader_kind::shaderc_tess_evaluation_shader;
		break;
	}
		
	CRITICAL_ERROR_LOG("Invalid shader stage flag")
}


std::shared_ptr<ShaderProgram> ShaderProgram::Create(const std::string& shader_name, bool isCompute)
{

	//Check if shader with that name exists, if it does return pointer to it
			//TODO

	//Else create new shader
	return std::make_shared<ShaderProgram>(shader_name, isCompute);

}



ShaderProgram::ShaderProgram(const std::string& shader_name, bool isCompute)
{

	

	compileOrGetSpirV(shader_name, isCompute);

	createShaderModules();
	
	reflectShaderModules();

	createDescriptorSetLayouts();

}


void ShaderProgram::compileOrGetSpirV(const std::string& shader_name, bool isCompute)
{

	std::unordered_map<ShaderStageFlag, std::string> shader_names;

	if (isCompute)
	{
		shader_names.emplace(ShaderStageFlag::Compute, shader_name + ".comp");
	}
	else
	{
		shader_names.emplace(ShaderStageFlag::Vertex, shader_name + ".vert");
		shader_names.emplace(ShaderStageFlag::Fragment, shader_name + ".frag");
		shader_names.emplace(ShaderStageFlag::Geometry, shader_name + ".geom");
		shader_names.emplace(ShaderStageFlag::TessC, shader_name + ".tesc");
		shader_names.emplace(ShaderStageFlag::TessE, shader_name + ".tese");

	}
	
	
	for (auto& it : shader_names)
	{
		//Find shader in cache folder
		std::string cached_shader = getShaderCacheDirectory() + it.second + ".spv";
		std::ifstream in(cached_shader, std::ios::in | std::ios::binary);
		if (in.is_open()) //TODO: if open and shader file has not changed since last time
		{
			in.seekg(0, std::ios::end);
			auto file_size = in.tellg();
			in.seekg(0, std::ios::beg);

			auto& data = m_spirv_source[it.first];
			data.resize(file_size / sizeof(uint32_t));

			in.seekg(0);
			in.read((char*)data.data(), file_size);
		}
		else //if not in spirv cache look for glsl shader.
		{
			std::string glsl_shader = getShaderDirectory() + it.second;
			std::ifstream in(glsl_shader, std::ios::in | std::ios::binary);
			std::string raw_glsl;

		
			shaderc::Compiler compiler;
			shaderc::CompileOptions options;

			const bool optimize = false;
			if (optimize)
			{
				options.SetOptimizationLevel(shaderc_optimization_level_size);
			}

			shaderc_util::FileFinder fileFinder;
			options.SetIncluder(std::make_unique<glslc::FileIncluder>(&fileFinder));
			options.SetGenerateDebugInfo();
			

			if (in) //if found compile and cache spirv
			{

				

				in.seekg(0, std::ios::end);
				raw_glsl.resize(in.tellg());
				in.seekg(0, std::ios::beg);
				in.read(&raw_glsl[0], raw_glsl.size());
				in.close();

				//std::cout << "output 1:\n" << raw_glsl << std::endl;

				shaderc::PreprocessedSourceCompilationResult preprocessed = compiler.PreprocessGlsl(raw_glsl, shaderStageFlagToShaderc(it.first), glsl_shader.c_str(), options);
				
				if (preprocessed.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					CRITICAL_ERROR_LOG("Preprocess failed for file " + glsl_shader + ":\n" + preprocessed.GetErrorMessage());
				}

				std::string preprocessed_glsl = { preprocessed.cbegin(), preprocessed.cend() };
				
				//std::cout << "output 2:\n" << preprocessed_glsl << std::endl;

				shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(preprocessed_glsl, shaderStageFlagToShaderc(it.first), glsl_shader.c_str());
				if (result.GetCompilationStatus() != shaderc_compilation_status_success)
				{
					//handle errors
					CRITICAL_ERROR_LOG("Error compiling shaders: " + glsl_shader +".  \n" +	result.GetErrorMessage());
				}

				auto& data = m_spirv_source[it.first];
				data.assign(result.cbegin(), result.cend());

				//write file to cache
				std::ofstream out(cached_shader, std::ios::out | std::ios::binary);
				if (out.is_open())
				{
					auto& data = m_spirv_source[it.first];
					out.write((char*)data.data(), data.size() * sizeof(uint32_t));
					out.flush();
					out.close();
					CONSOLE_LOG("Writing compiled SPIRV shader: " + it.second);
				}

			}
			
		}

	}

	if (m_spirv_source.size() == 0)
	{
		CRITICAL_ERROR_LOG("Error building shaders. No shader exists with name: " + shader_name);
	}

}


void ShaderProgram::createShaderModules()
{

	//Create vulkan shader module
	for (auto& it : m_spirv_source)
	{

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.pNext = nullptr;

		//codeSize has to be in bytes
		create_info.codeSize = it.second.size() * sizeof(uint32_t);
		create_info.pCode = it.second.data();


		VkShaderModule shader_module;
		VkDevice device = RenderModule::getRenderBackend()->getDevice();
		VK_CHECK(vkCreateShaderModule(device, &create_info, nullptr, &shader_module));
		RenderModule::getRenderBackend()->pushToDeletionQueue([=]() {vkDestroyShaderModule(device, shader_module, nullptr); });
		m_shader_module.push_back(shader_module);


		//Create shader stage
		VkPipelineShaderStageCreateInfo stage_create_info = {};
		stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stage_create_info.pNext = nullptr;
		stage_create_info.stage = static_cast<VkShaderStageFlagBits>(it.first);

		//shader module
		stage_create_info.module = shader_module;

		//shader entry point
		stage_create_info.pName = "main";

		m_pipeline_stages.push_back(stage_create_info);

	}



	//reflectShaderModule(shader_spv, shader_type); //on reflect shader extract all sets and bindings etc. 
	//Then make a function clled build descriptor set layouts or something that sorts them and creates them and the pipeline layouts2


	
}



void ShaderProgram::reflectShaderModules()
{
	for (auto& spirv_source_it : m_spirv_source)
	{
		//Reflection
		spirv_cross::Compiler comp(std::move(spirv_source_it.second));

		// The SPIR-V is now parsed, and we can perform reflection on it.
		spirv_cross::ShaderResources resources = comp.get_shader_resources();

		//Uniform Buffers
		for (auto& resource : resources.uniform_buffers)
		{
			unsigned set = comp.get_decoration(resource.id, spv::DecorationDescriptorSet);
			unsigned binding = comp.get_decoration(resource.id, spv::DecorationBinding);
			if (m_bindings[set].find(binding) == m_bindings[set].end()) //if not found
			{
				auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<VkShaderStageFlagBits>(spirv_source_it.first));
				m_bindings[set].insert({binding, decriptor_binding});
			} 
			else
			{
				//Add stage flag
				addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(spirv_source_it.first)); //UNTESTED MIGHT NOT WORK
			
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
				auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, static_cast<VkShaderStageFlagBits>(spirv_source_it.first));
				m_bindings[set].insert({ binding, decriptor_binding });
			}
			else
			{
				//Add stage flag
				addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(spirv_source_it.first)); //UNTESTED MIGHT NOT WORK

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
				auto decriptor_binding = getDescriptorSetLayoutBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<VkShaderStageFlagBits>(spirv_source_it.first));
				m_bindings[set].insert({ binding, decriptor_binding });
			}
			else
			{
				//Add stage flag
				addStageFlagToBinding(m_bindings[set][binding], static_cast<VkShaderStageFlagBits>(spirv_source_it.first)); //UNTESTED MIGHT NOT WORK

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
			constant_range.stageFlags = static_cast<VkShaderStageFlagBits>(spirv_source_it.first);
			constant_range.offset = 0;
			constant_range.size = comp.get_declared_struct_size(type);

			m_push_constants.push_back(constant_range);
		}


		//TODO: subpass attachments, etc etc

	}
	
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