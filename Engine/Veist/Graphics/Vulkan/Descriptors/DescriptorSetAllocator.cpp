#include "pch.h"


#include "DescriptorSetAllocator.h"

#include "Veist/Graphics/RenderModule.h"

namespace Veist
{
	
	static VkDescriptorPool createPool(const DescriptorSetLayout& layout, uint32_t descriptor_set_count, uint32_t descriptor_count, VkDescriptorPoolCreateFlags flags)
	{

		//TODO rework this to only allocate enough descriptors as neded not more
		std::vector<VkDescriptorPoolSize> sizes;
		sizes.reserve(layout.m_bindings.size());
		for (auto binding : layout.m_bindings)
		{
			VkDescriptorPoolSize size;
			size.descriptorCount = descriptor_count * descriptor_set_count;
			size.type = binding.descriptorType;
			sizes.emplace_back(size);
		}
		



		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = flags;
		pool_info.maxSets = descriptor_set_count;
		pool_info.poolSizeCount = (uint32_t)sizes.size();
		pool_info.pPoolSizes = sizes.data();

		VkDescriptorPool descriptorPool;

		VkDevice device = RenderModule::getBackend()->getDevice();
		vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

		return descriptorPool;

	}


	DescriptorSetLayout::DescriptorSetLayout(const std::vector<Descriptor>& descriptor_list)
	{
		//TODO support inline blocks?
		m_bindings.reserve(descriptor_list.size());
		uint32_t count = 0;
		for (auto descriptor : descriptor_list)
		{
			VkDescriptorSetLayoutBinding binding;
			binding.binding = count++;
			binding.descriptorType = descriptor.type();
			binding.stageFlags = descriptor.stageFlags();
			binding.descriptorCount = 1; //if inline block this is size of of block in bytes

			m_bindings.emplace_back(binding);
		}
	}



	bool DescriptorSetLayout::operator==(const DescriptorSetLayout& other) const
	{

		if (m_bindings.size() != other.m_bindings.size())
		{
			return false;
		}
		else
		{
			//Compare each binding
			for (int i = 0; i < m_bindings.size(); i++)
			{

				if (m_bindings[i].binding != other.m_bindings[i].binding)
				{
					return false;
				}
				if (m_bindings[i].descriptorType != other.m_bindings[i].descriptorType)
				{
					return false;
				}
				if (m_bindings[i].descriptorCount != other.m_bindings[i].descriptorCount)
				{
					return false;
				}
				if (m_bindings[i].stageFlags != other.m_bindings[i].stageFlags)
				{
					return false;
				}
			}
			return true;
		}
	}

	//From vkguide.dev
	size_t DescriptorSetLayout::hash() const
	{
		size_t result = std::hash<size_t>()(m_bindings.size());

		for (const VkDescriptorSetLayoutBinding& b : m_bindings)
		{
			//pack the binding data into a single int64. Not fully correct but it's ok
			size_t binding_hash = b.binding | b.descriptorType << 8 | b.descriptorCount << 16 | b.stageFlags << 24; //TODO:With shader stage flag all this might be wonky. check

			//shuffle the packed binding data and xor it with the main hash. Could also be m * H(A) + H(B) etc
			result ^= std::hash<size_t>()(binding_hash);
		}

		return result;
	}


	VkDescriptorSetLayout DescriptorSetAllocator::addDescriptorPool(std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		DescriptorSetLayout layout{bindings};

		auto it = m_descriptor_pools.find(layout);
		if (it == m_descriptor_pools.end())//if pool not in map
		{
			auto& map_entry = m_descriptor_pools.emplace(layout, layout);
			
			return map_entry.first->second.descriptorSetLayout();
		}

		return it->second.descriptorSetLayout();
	}


	DescriptorSetPool::DescriptorSetPool(const DescriptorSetLayout& layout)
	{

		//Create descriptor layout
		VkDescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = layout.m_bindings.size();
		layout_info.flags = 0;
		layout_info.pBindings = layout.m_bindings.data();
		layout_info.pNext = nullptr;
		

		

		VkDevice device = RenderModule::getBackend()->getDevice();
		VkDescriptorSetLayout set_layout;
		VK_CHECK(vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &set_layout));
		RenderModule::getBackend()->pushToDeletionQueue([device, set_layout]() {vkDestroyDescriptorSetLayout(device, set_layout, nullptr); });
		
		m_layout = set_layout;

		//Allocate pool
		m_pool = createPool(layout, descriptor_pool_size, descriptor_type_size, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		
		//Allocate Descriptor sets
		std::array<VkDescriptorSetLayout, descriptor_pool_size> layouts;
		std::fill_n(layouts.begin(), descriptor_pool_size, m_layout);

		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.pNext = nullptr;

		alloc_info.pSetLayouts = layouts.data();
		alloc_info.descriptorPool = m_pool;
		alloc_info.descriptorSetCount = descriptor_pool_size;

		//Allocate descriptor sets
		VK_CHECK(vkAllocateDescriptorSets(device, &alloc_info, m_descriptor_sets.data()));


		//Set which descriptors are free
		m_free_descriptors.set();
		m_next_free_idx = 0;

	}


	DescriptorSetPool::~DescriptorSetPool()
	{
		VkDevice device = RenderModule::getBackend()->getDevice();
		vkDestroyDescriptorPool(device, m_pool, nullptr);
		//vkDestroyDescriptorSetLayout(device, m_layout, nullptr);//In deletion queue
		
	}



	uint32_t DescriptorSetPool::getFreeDescriptorSetIndex()
	{
		uint32_t index = m_next_free_idx;

		m_free_descriptors.set(m_next_free_idx, false);

		bool next_free_found = false;
		while (!next_free_found) //I could use a queue here isntead of iterating through bitset finding next free index but its fine
		{
			m_next_free_idx++;

			if (m_next_free_idx >= descriptor_pool_size)
			{
				CRITICAL_ERROR_LOG("Descriptor pool full, could not find free descriptor");
			}

			next_free_found = m_free_descriptors.test(m_next_free_idx);

		}

		return index;

	}



	void DescriptorSetPool::recycleDescriptor(uint32_t index)
	{
		
		uint32_t frames_in_flight = RenderModule::getBackend()->getSwapchainImageCount();
		
		m_descriptor_recycle_list.emplace_back(0, index);


		for (auto& it = m_descriptor_recycle_list.begin(); it != m_descriptor_recycle_list.end(); )
		{
			if (it->first++ >= frames_in_flight)
			{
				uint32_t desc_index = it->second;
				m_free_descriptors.set(desc_index, true);
				if (m_next_free_idx > desc_index)
				{
					m_next_free_idx = desc_index;
				}
				
				it = m_descriptor_recycle_list.erase(it);
			}
			else
			{
				it++;
			}
		}


		
	}





	DescriptorSet::DescriptorPoolData DescriptorSetAllocator::createDescriptorSet(std::vector<Descriptor>& descriptor_bindings)
	{

		DescriptorSetLayout layout{descriptor_bindings};
		auto it = m_descriptor_pools.find(layout);

		if (it == m_descriptor_pools.end())
		{
			CRITICAL_ERROR_LOG("No shader uses descriptor layout for this pass and therefore no layout has been created with reflection")
		}
		else
		{
			DescriptorSet::DescriptorPoolData pool_data;
			pool_data.m_pool = &it->second;
			pool_data.m_index = it->second.m_next_free_idx;

			DescriptorSetPool& pool = it->second;
			uint32_t desc_set_idx = pool.getFreeDescriptorSetIndex();
			
			std::vector<VkWriteDescriptorSet> m_writes;
			m_writes.reserve(descriptor_bindings.size());

			uint32_t binding = 0;
			for (auto& descriptor : descriptor_bindings)
			{
				VkWriteDescriptorSet set_write = {};
				set_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set_write.descriptorCount = 1; //If inline blocks supported this is size in bytes
				set_write.descriptorType = descriptor.type();
				set_write.dstSet = pool.m_descriptor_sets[desc_set_idx];
				set_write.dstBinding = binding;
				set_write.pImageInfo = &descriptor.info().image_info;
				set_write.pBufferInfo = &descriptor.info().buffer_info;
				
				m_writes.emplace_back(set_write);

				binding++;
			}

			VkDevice device = RenderModule::getBackend()->getDevice();
			vkUpdateDescriptorSets(device, m_writes.size(), m_writes.data(), 0, nullptr);


			return pool_data;
		}
	}




	DescriptorSetAllocator::~DescriptorSetAllocator()
	{
		
	}



/*
DescriptorSetAllocator::DescriptorSetAllocator() : m_layout_cache(std::make_unique<DescriptorSetLayoutCache>())
{

}



bool DescriptorSetAllocator::allocateDescriptorSet(DescriptorSet& descriptor_set) 
{

	if (m_current_pool == nullptr) {
		m_current_pool = getPool();
		m_used_pools.push_back(m_current_pool);
	}

	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.pNext = nullptr;

	VkDescriptorSetLayout layout = descriptor_set.descriptorSetLayout();
	alloc_info.pSetLayouts = &layout;
	alloc_info.descriptorPool = m_current_pool;
	alloc_info.descriptorSetCount = 1;

	//attempt allocate descriptor set
	VkDevice device = RenderModule::getBackend()->getDevice();
	VkResult alloc_result = vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set.descriptorSet());
	bool reallocate_pool = false;


	switch (alloc_result) {
		case VK_SUCCESS:
			return true;
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			reallocate_pool = true;
			break;
		
		default:
			return false; //failure
	}


	if (reallocate_pool) {
		//allocate new pool and retry
		m_current_pool = getPool();
		m_used_pools.push_back(m_current_pool);

		alloc_result = vkAllocateDescriptorSets(device, &alloc_info, &descriptor_set.descriptorSet());

		if (alloc_result == VK_SUCCESS) {
			return true;
		}
	}


	return false;
}



static VkDescriptorPool createPool(const DescriptorSetAllocator::PoolSizes& pool_sizes, int descriptor_count, VkDescriptorPoolCreateFlags flags)
{
	std::vector<VkDescriptorPoolSize> sizes;
	sizes.reserve(pool_sizes.sizes.size());
	
	for (auto size : pool_sizes.sizes) {
		sizes.push_back({ size.first, uint32_t(size.second * descriptor_count) });
	}

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = flags;
	pool_info.maxSets = descriptor_count;
	pool_info.poolSizeCount = (uint32_t)sizes.size();
	pool_info.pPoolSizes = sizes.data();

	VkDescriptorPool descriptorPool;
	
	VkDevice device = RenderModule::getBackend()->getDevice();
	vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);

	return descriptorPool;

}


VkDescriptorPool DescriptorSetAllocator::getPool()
{
	
	if (m_free_pools.size() > 0)
	{
		//grab pool
		VkDescriptorPool pool = m_free_pools.back();
		m_free_pools.pop_back();
		return pool;
	}
	else
	{
		//create new pool if none availible
		 
		return createPool( m_descriptor_sizes, 1000, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);
	}

}


void DescriptorSetAllocator::resetPools() {
	
	//reset every pool
	VkDevice device = RenderModule::getBackend()->getDevice();
	for (auto pool : m_used_pools) {
		vkResetDescriptorPool(device, pool, 0);
	}

	//move pools to free pools array
	m_free_pools = m_used_pools;
	m_used_pools.clear();

	//set current pool to null
	m_current_pool = nullptr;
}


void DescriptorSetAllocator::cleanup()
{

	VkDevice device = RenderModule::getBackend()->getDevice();
	for (auto pool : m_free_pools)
	{
		vkDestroyDescriptorPool(device, pool, nullptr);
	}
	for (auto pool : m_used_pools)
	{
		vkDestroyDescriptorPool(device, pool, nullptr);
	}

}

*/
}