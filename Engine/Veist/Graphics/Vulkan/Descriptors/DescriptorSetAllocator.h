#pragma once

#include <vulkan/vulkan.h>
#include "Veist/Graphics/Vulkan/Descriptors/DescriptorSet.h"
#include "Veist/Graphics/Vulkan/Descriptors/DescriptorSetLayoutCache.h"


namespace Veist
{
	//Descriptor set allocator design inspired from vkguide.dev and 'yave' by gan74
	

	struct DescriptorSetLayout
	{
		DescriptorSetLayout(const std::vector<Descriptor>& bindings);
		DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& bindings) : m_bindings(bindings) {};

		std::vector<VkDescriptorSetLayoutBinding> m_bindings; //TODO: maybe shouldnt be a vector (dynamic alloc every pushback)

		bool operator==(const DescriptorSetLayout& other) const;

		size_t hash() const;
	};



	class DescriptorSetPool
	{
	public:
		static constexpr size_t descriptor_pool_size = 128;
		static constexpr size_t descriptor_type_size = 16; //How many descriptors per descriptor set in the pool
		DescriptorSetPool(const DescriptorSetLayout& layout);
		~DescriptorSetPool();


		uint32_t getFreeDescriptorSetIndex();

		VkDescriptorSet descriptorSet(uint32_t index) const { return m_descriptor_sets[index]; }
		VkDescriptorSetLayout descriptorSetLayout() const { return m_layout; }

		void recycleDescriptor(uint32_t index);


	private:
		
		friend class DescriptorSetAllocator;

		std::bitset<descriptor_pool_size> m_free_descriptors;
		uint32_t m_next_free_idx;
		
		VkDescriptorSetLayout m_layout;
		VkDescriptorPool m_pool;
		std::array<VkDescriptorSet, descriptor_pool_size> m_descriptor_sets;


		//std::list<std::pair<uint32_t, uint32_t>> m_descriptor_recycle_list;

		//std::vector<VkDescriptorSetLayoutBinding> TESTLAYOUT;

	};


	class DescriptorSetAllocator
	{
	public:

		DescriptorSetAllocator() = default;
		~DescriptorSetAllocator();

		void cleanup() { m_descriptor_pools.clear(); };

		VkDescriptorSetLayout addDescriptorPool(std::vector<VkDescriptorSetLayoutBinding>& bindings);
		
		DescriptorSet::DescriptorPoolData createDescriptorSet(std::vector<Descriptor>& descriptor_bindings);
	
	private:

		struct DescriptorHash
		{
			size_t operator()(const DescriptorSetLayout& info) const
			{
				return info.hash();
			};

		};

		//When searching for descriptorlayout ensure that only shader reflection. inserts new pools for new layouts into the map 
		std::unordered_map<DescriptorSetLayout, DescriptorSetPool, DescriptorHash> m_descriptor_pools;


	};


//TODO rework this class entirely. To hold DescriptorPools for each DescriptorSetLayout. Then allocate descriptor sets based on that and recycle them instead of resetting pool. 
//Try to also cache uses of descriptor sets that have not changed across frames
/*
class DescriptorSetAllocator
{
public:
	
	DescriptorSetAllocator();

	void cleanup();

	bool allocateDescriptorSet(DescriptorSet& descriptor_set);

	void resetPools();

	DescriptorSetLayoutCache* layoutCache() { return m_layout_cache.get(); };


	//from vkguide.dev (should edit it later to fit my needs exactly)
	struct PoolSizes {
		std::vector<std::pair<VkDescriptorType, float>> sizes =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 2.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 2.f },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
		};
	};

private:

	VkDescriptorPool getPool();

	PoolSizes m_descriptor_sizes;
	VkDescriptorPool m_current_pool{nullptr};

	std::vector<VkDescriptorPool> m_free_pools;
	std::vector<VkDescriptorPool> m_used_pools;
	std::unique_ptr<DescriptorSetLayoutCache> m_layout_cache;
	
};
*/
}