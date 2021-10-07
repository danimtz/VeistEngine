

#include "CommandPool.h"
#include "Engine/Renderer/RenderModule.h"

CommandPool::CommandPool(int thread_id) : m_thread_id(thread_id)
{

	uint32_t graphics_family_idx = RenderModule::getRenderBackend()->getGraphicsFamily();
	VkDevice device = RenderModule::getRenderBackend()->getDevice();

	VkCommandPoolCreateInfo pool_create_info = {};
	pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	pool_create_info.queueFamilyIndex = graphics_family_idx;
	pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	pool_create_info.pNext = nullptr;


	VK_CHECK(vkCreateCommandPool(device, &pool_create_info, nullptr, &m_pool));

	VkCommandPool pool = m_pool;
	RenderModule::getRenderBackend()->pushToDeletionQueue([=]() { vkDestroyCommandPool(device, pool, nullptr);	});

}



void CommandPool::resetPool()
{
	VkDevice device = RenderModule::getRenderBackend()->getDevice();
	vkResetCommandPool(device, m_pool, 0);
}