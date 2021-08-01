#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include "Engine/Logger.h"
#include "Engine/Renderer/RenderModule.h"
//#include "Engine/Renderer/ShaderAndPipelines/GraphicsPipeline.h"

enum class VulkanShaderType : uint32_t {
    None = 0,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    Geomery = VK_SHADER_STAGE_GEOMETRY_BIT,
    Compute = VK_SHADER_STAGE_COMPUTE_BIT
};

//Vulkan shader implementation. Push constants   shjould maybe be handled from this? maybe descriptors sets, maybe that goes in material. To be determined. Leaving the class empty for now
class VulkanShaderProgram
{

public:

    VulkanShaderProgram(std::string file_path, std::string folder_path);

    

    std::vector<VkShaderModule>& shaderModules() {return m_shader_module; };
    std::vector<VkPipelineShaderStageCreateInfo>& pipelineStages() {	return m_pipeline_stages; };
   
   
   static std::shared_ptr<VulkanShaderProgram> Create(std::string shader_name, std::string folder_path = "..\\..\\src\\Shaders\\");

private:

    void createShaderModule(const char* file_path, VulkanShaderType shader_type);
    void reflectShader(){};

    std::vector<VkShaderModule>						m_shader_module;
    std::vector<VkPipelineShaderStageCreateInfo>	m_pipeline_stages;
    std::vector<VkDescriptorSetLayout>              m_descriptor_layouts;
    std::vector<VkPushConstantRange>                m_push_constants;


};

