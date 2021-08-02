#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
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
    std::vector<VkDescriptorSetLayout>& descriptorLayouts() { return m_descriptor_layouts; };
    std::vector<VkPushConstantRange>& pushConstantRanges() { return m_push_constants; };
   
   
   static std::shared_ptr<VulkanShaderProgram> Create(std::string shader_name, std::string folder_path = "..\\..\\src\\Shaders\\");

private:

    void createShaderModule(const char* file_path, VulkanShaderType shader_type);
    void reflectShaderModule(std::vector<uint32_t>& buffer, VulkanShaderType shader_type);
    void createDescriptorSetLayouts();

    static constexpr int MAX_DESCRIPTOR_SETS = 4;//find better palce for this

    std::map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings[MAX_DESCRIPTOR_SETS];

    std::vector<VkShaderModule>						m_shader_module;
    std::vector<VkPipelineShaderStageCreateInfo>	m_pipeline_stages;
    std::vector<VkDescriptorSetLayout>              m_descriptor_layouts;
    std::vector<VkPushConstantRange>                m_push_constants;


};

