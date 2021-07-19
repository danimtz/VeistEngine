#include "GraphicsPipeline.h"


#include "Platform/Vulkan/VulkanGraphicsPipeline.h"

std::shared_ptr<GraphicsPipeline> GraphicsPipeline::Create(std::string shader_name, std::string folder_path, const VertexDescription& vertex_description)
{
	
	//Add support for other API pipelines. 

	//TODO solve PROBLEM: VulkanGraphicsPipeline has default constructor for a bunch more arguemnts that cant be currently accesed through the abstraction
	//POSSIBLE SOLUTION: -extend GraphicsPipeline base class to include these arguemnts as their own abstracted form somehow:
	//			include them as a list of settings pipeline_settings. then in VulkanGraphicsPipeline constructor go to a function that parses these abstract settings into vulkan pipeline settings

	return std::make_shared<VulkanGraphicsPipeline>(shader_name, folder_path, vertex_description);

}