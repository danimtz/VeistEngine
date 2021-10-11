#pragma once

#include <vulkan/vulkan.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include "Engine/Logger.h"

enum class ShaderStageFlag : uint32_t {
    None = 0,
    Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
    Vertex = VK_SHADER_STAGE_VERTEX_BIT,
    Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
    TessC = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
    TessE = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
    Compute = VK_SHADER_STAGE_COMPUTE_BIT
};



class ShaderProgram
{
public:

    static std::shared_ptr<ShaderProgram> Create(std::string shader_name);

    ShaderProgram(std::string file_path);

    std::vector<VkShaderModule>& shaderModules() { return m_shader_module; };
    std::vector<VkPipelineShaderStageCreateInfo>& pipelineStages() { return m_pipeline_stages; };
    std::vector<VkDescriptorSetLayout>& descriptorLayouts() { return m_descriptor_layouts; };
    std::vector<VkPushConstantRange>& pushConstantRanges() { return m_push_constants; };
   
   
  

private:
    

    void compileOrGetSpirV(const std::string shader_name);
    void createShaderModules();
    void reflectShaderModules();
    void createDescriptorSetLayouts();

    static constexpr int MAX_DESCRIPTOR_SETS = 4;//find better palce for this

    std::map<uint32_t, VkDescriptorSetLayoutBinding> m_bindings[MAX_DESCRIPTOR_SETS];

    std::vector<VkShaderModule>						m_shader_module;
    std::vector<VkPipelineShaderStageCreateInfo>	m_pipeline_stages;
    std::vector<VkDescriptorSetLayout>              m_descriptor_layouts;
    std::vector<VkPushConstantRange>                m_push_constants;


    std::unordered_map<ShaderStageFlag, std::string> m_shader_source_code;
    std::unordered_map<ShaderStageFlag, std::vector<uint32_t>> m_spirv_source;
    

};

