#pragma once

#include "RenderGraphPassBuilder.h"

namespace Veist
{

	
	
	class RenderGraph
	{
	public:
	
	
		
		RenderGraphPassBuilder addPass(std::string_view name);

	
	private:


		std::vector<std::unique_ptr<RenderGraphPass>> m_passes;
		std::vector<std::unique_ptr<RenderGraphResource>> m_resources;

		//std::shared_ptr<RenderGraphPhysResourcePool> m_resource_pool;

		std::unordered_map<std::string, uint32_t> m_resource_to_idx_map;
		std::unordered_map<std::string, uint32_t> m_pass_to_idx_map;
	};




}
