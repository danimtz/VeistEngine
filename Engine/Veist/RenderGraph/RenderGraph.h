#pragma once

#include "RendererUniforms.h"
#include "RenderGraphPassBuilder.h"
#include "RenderGraphResourcePool.h"

namespace Veist
{

	
	
	class RenderGraph
	{
	public:
	
		RenderGraph(std::shared_ptr<RenderGraphResourcePool> pool) : m_resource_pool(pool){}
		
		RenderGraphPassBuilder addPass(std::string_view name);

		

		bool setOutputBuffer(const std::string& name);

		RenderGraphBufferResource* getOrAddBufferResource(const std::string& name);
		RenderGraphImageResource* getOrAddImageResource(const std::string& name);
	
	private:


		std::vector<std::unique_ptr<RenderGraphPass>> m_passes;
		std::vector<std::unique_ptr<RenderGraphResource>> m_resources;

		std::shared_ptr<RenderGraphResourcePool> m_resource_pool;

		std::unordered_map<std::string, uint32_t> m_resource_to_idx_map;
		std::unordered_map<std::string, uint32_t> m_pass_to_idx_map;
	};




}
