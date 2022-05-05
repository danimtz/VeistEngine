#include <pch.h>
#include "VulkanGarbageCollector.h"

#include "Veist/Graphics/RenderModule.h"

namespace Veist
{

	void VulkanGarbageCollector::registerDestruction(VkFence fence, std::function<void()> func)
	{

		m_deletion_list.emplace_back(fence, func);

	}


	void VulkanGarbageCollector::destroyFrameResources()
	{
		//Once command buffer is finished move buffers and images to unused pool
		VkDevice device = RenderModule::getBackend()->getDevice();
		for (auto& it = m_deletion_list.begin(); it != m_deletion_list.end(); )
		{
			if (vkGetFenceStatus(device, it->first) == VK_SUCCESS)
			{
				it->second();//call function
				it = m_deletion_list.erase(it);
			}
			else
			{
				it++;
			}
		}
		

	}


}
