#pragma once

#include "RendererUniforms.h"
#include "PassBuilder.h"
#include "ResourcePool.h"

namespace Veist
{
namespace RenderGraph
{



	
	class RenderGraph
	{
	public:
	
		RenderGraph(std::shared_ptr<ResourcePool> pool) : m_resource_pool(pool){}
		~RenderGraph();

		PassBuilder addPass(std::string_view name);

		void execute(CommandBuffer& cmd);

		bool setBackbuffer(const std::string& name);

		std::pair<bool, BufferResource*> addBufferResource(const std::string& name);
		std::pair<bool, ImageResource*> addImageResource(const std::string& name);
		Resource* getResource(uint32_t index);

		ResourcePool* resourcePool() { return m_resource_pool.get();};

	private:

		bool validateGraph();
		void setupGraphPassOrder(std::stack<uint32_t>& next_passes, std::stack<uint32_t>& next_resources);
		void setupPhysicalResources();
		void aliasResources();
		void allocatePhysicalResources();
		void createBarriers(uint32_t pass_idx, CommandBuffer& cmd);

		std::stack<uint32_t> m_next_passes;
		std::stack<uint32_t> m_next_resources;

	private:

		friend class PassBuilder;

		int32_t m_backbuffer_idx{-1};

		std::vector<std::unique_ptr<RenderGraphPass>> m_passes;
		std::vector<std::unique_ptr<Resource>> m_resources;

		std::list<std::unique_ptr<PhysicalResource>> m_physical_resources;

		std::shared_ptr<ResourcePool> m_resource_pool;

		std::unordered_map<std::string, uint32_t> m_resource_to_idx_map;
		std::unordered_map<std::string, uint32_t> m_pass_to_idx_map;

		//TODO intermediary between physical index in resources and actual index of image in resource pool. use this for aliasing MUST CHAGNE HOW ALLOCATE PHYSICAL RESOURCES WORKS
		//std::unordered_map<uint32_t, uint32_t> m_phys_index_to_res_pool_idx_map;


		std::vector<uint32_t> m_pass_stack;

		
		
	};



}
}
