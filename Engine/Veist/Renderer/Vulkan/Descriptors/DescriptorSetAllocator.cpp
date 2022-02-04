#include "pch.h"


#include "DescriptorSetAllocator.h"

#include "Veist/Renderer/RenderModule.h"

namespace Veist
{

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
		return createPool( m_descriptor_sizes, 1000, 0);
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


}