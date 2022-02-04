#include "pch.h"

#include "CommandBuffer.h"
#include "CommandPool.h"

#include "Veist/Renderer/RenderModule.h"



namespace Veist
{


CommandPool::CommandPool(int thread_id) : m_thread_id(thread_id)
{

	uint32_t graphics_family_idx = RenderModule::getBackend()->getGraphicsFamily();
	VkDevice device = RenderModule::getBackend()->getDevice();

	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.queueFamilyIndex = graphics_family_idx;
	pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_create_info.pNext = nullptr;


	VK_CHECK(vkCreateCommandPool(device, &pool_create_info, nullptr, &m_pool));

	VkCommandPool pool = m_pool;
	RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroyCommandPool(device, pool, nullptr);	});


	

}



CommandBuffer CommandPool::allocateCommandBuffer(bool begin_cmd_buffer)
{

	VkDevice device = RenderModule::getBackend()->getDevice();

	//Create Fence
	VkFence fence;
	m_fences.push_back(fence);

	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.pNext = nullptr;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &fence));
	
	RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroyFence(device, fence, nullptr); });

	return CommandBuffer(this, fence, begin_cmd_buffer);
}


void CommandPool::resetPool()
{
	VkDevice device = RenderModule::getBackend()->getDevice();
	vkResetCommandPool(device, m_pool, 0);
}

}