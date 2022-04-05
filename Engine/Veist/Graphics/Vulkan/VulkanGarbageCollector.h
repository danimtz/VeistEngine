#pragma once


#include <vulkan/vulkan.h>

namespace Veist
{

	class VulkanGarbageCollector
	{
	public:
		
		VulkanGarbageCollector() = default;

		void registerDestruction(VkFence fence, std::function<void()> func);
		void destroyFrameResources();


	private:

		std::list<std::pair<VkFence, std::function<void()>>> m_deletion_list;

	};


}



