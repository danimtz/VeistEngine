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
		~RenderGraph();

		RenderGraphPassBuilder addPass(std::string_view name);

		void execute(CommandBuffer& cmd);

		bool setBackbuffer(const std::string& name);

		RenderGraphBufferResource* getOrAddBufferResource(const std::string& name);
		RenderGraphImageResource* getOrAddImageResource(const std::string& name);
		RenderGraphResource* getResource(uint32_t index);

		RenderGraphResourcePool* resourcePool() { return m_resource_pool.get();};

	private:

		bool validateGraph();
		void setupGraphPassOrder(std::stack<uint32_t>& next_passes, std::stack<uint32_t>& next_resources);
		void allocatePhysicalResources();

		std::stack<uint32_t> m_next_passes;
		std::stack<uint32_t> m_next_resources;

	private:

		int32_t m_backbuffer_idx{-1};

		std::vector<std::unique_ptr<RenderGraphPass>> m_passes;
		std::vector<std::unique_ptr<RenderGraphResource>> m_resources;


		std::shared_ptr<RenderGraphResourcePool> m_resource_pool;

		std::unordered_map<std::string, uint32_t> m_resource_to_idx_map;
		std::unordered_map<std::string, uint32_t> m_pass_to_idx_map;

		std::vector<uint32_t> m_pass_stack;

		
		
	};




}
