#include "pch.h"

#include "Veist/Graphics/RenderModule.h"

#include "Swapchain.h"


namespace Veist
{


static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

	for (const auto& format : formats) {
		/*if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}*/
		if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}


	//if best not found return the first one in list
	return formats[0];

}


static bool checkSwapchainOutOfDate(VkResult result)
{
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		return true;
	}

	VK_CHECK(result);

	return false;
}


static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& present_modes) {

	for (const auto& present_mode : present_modes) {
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return present_mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}


Swapchain::Swapchain(const VkExtent2D& extent) : Swapchain(extent, nullptr)
{
}


Swapchain::Swapchain(const VkExtent2D& extent, std::shared_ptr<Swapchain> old_swapchain) : m_extent(extent), m_old_swapchain(old_swapchain)
{

	createSwapchain();
	createSyncStructures();

	m_old_swapchain.reset(); //delete old swapchain
}


Swapchain::~Swapchain()
{
	VkDevice device = RenderModule::getBackend()->getDevice();
	m_images.clear();
	vkDestroySwapchainKHR(device, vk_swapchainKHR(), nullptr);
}



void Swapchain::createSyncStructures()
{
	//create fence
	VkFenceCreateInfo fence_create_info = {};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.pNext = nullptr;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//create semaphore
	VkSemaphoreCreateInfo semaphore_create_info = {};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.pNext = nullptr;
	semaphore_create_info.flags = 0;

	VkDevice device = RenderModule::getBackend()->getDevice();

	for (int i = 0; i < m_images.size(); i++) {
		
		auto& sync_structs = m_sync_structs.emplace_back();

		VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &sync_structs.m_render_fence));
		RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroyFence(device, sync_structs.m_render_fence, nullptr); });

		VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &sync_structs.m_present_semaphore));
		VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &sync_structs.m_render_semaphore));
		RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroySemaphore(device, sync_structs.m_present_semaphore, nullptr); });
		RenderModule::getBackend()->pushToDeletionQueue([=]() { vkDestroySemaphore(device, sync_structs.m_render_semaphore, nullptr); });
	}

}



void Swapchain::createSwapchain()
{

	VkSurfaceKHR surface = RenderModule::getBackend()->getSurface();
	GPUinfo_t gpu = RenderModule::getBackend()->getGPUinfo();
	GLFWwindow* window = RenderModule::getBackend()->getWindow();

	VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(gpu.surface_formats);
	m_format = {surface_format.format};
	VkPresentModeKHR present_mode = chooseSwapPresentMode(gpu.present_modes);

	m_image_count = std::max<uint32_t>(gpu.surface_capabilities.minImageCount + 1, FRAME_OVERLAP_COUNT);
	if (gpu.surface_capabilities.maxImageCount > 0 && m_image_count > gpu.surface_capabilities.maxImageCount) {
		m_image_count = gpu.surface_capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = surface;
	create_info.minImageCount = m_image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = m_extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

	/*
	if (m_graphics_family_idx != m_present_family_idx) {

		uint32_t queue_family_indeces[] = { m_graphics_family_idx, m_present_family_idx };
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indeces;
	*/
	create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;


	create_info.preTransform = gpu.surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = (m_old_swapchain == nullptr) ? VK_NULL_HANDLE : m_old_swapchain->vk_swapchainKHR();

	VkDevice device = RenderModule::getBackend()->getDevice();


	VK_CHECK(vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swapchain));

	VkSwapchainKHR swapchain = m_swapchain;

	//Manually delete swapchain later in destructor
	//RenderModule::getBackend()->pushToSwapchainDeletionQueue([=]() {vkDestroySwapchainKHR(device, swapchain, nullptr); });


	//Get swapchain images
	std::vector<VkImage> swapchain_vkimages;
	vkGetSwapchainImagesKHR(device, m_swapchain, &m_image_count, nullptr);
	swapchain_vkimages.resize(m_image_count);
	vkGetSwapchainImagesKHR(device, m_swapchain, &m_image_count, swapchain_vkimages.data());


	ImageProperties swapchain_img_properties{ {m_extent.width, m_extent.height}, {m_format} };

	//Create image views
	for (size_t i = 0; i < m_image_count; i++) {
		m_images.emplace_back( std::make_unique<SwapchainImage>(swapchain_vkimages[i], swapchain_img_properties) );
	}

	
}






void Swapchain::beginNextFrame()
{
	VkDevice device = RenderModule::getBackend()->getDevice();
	uint32_t frame_count = RenderModule::getBackend()->getFrameNumber();
	
	VK_CHECK(vkWaitForFences(device, 1, &RenderModule::getBackend()->getCurrentCmdBuffer().fence(), true, 1000000000)); //1 second timeout 1000000000ns
	VK_CHECK(vkResetFences(device, 1, &RenderModule::getBackend()->getCurrentCmdBuffer().fence()));

	//request next image from swapchain, 1 second timeout
	VK_CHECK(vkAcquireNextImageKHR(device, vk_swapchainKHR(), 1000000000, currentSyncStructures(frame_count).m_present_semaphore, nullptr, &m_img_idx));

}



void Swapchain::present(const CommandBuffer& cmd_buffer)
{
	uint32_t frame_count = RenderModule::getBackend()->getFrameNumber();//TODO: maybe frame counter should be kept inside swapchain not render backend?


	VkCommandBuffer command_buffer = cmd_buffer.vk_commandBuffer();

	

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = nullptr;

	VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	submit_info.pWaitDstStageMask = &wait_stage;

	//Wait on the m_present_semaphore, that semaphore is signallled when swapchain ready
	submit_info.waitSemaphoreCount = 1;
	submit_info.pWaitSemaphores = &currentSyncStructures(frame_count).m_present_semaphore;

	//Signal the m_render_semaphore, to signal that rendering has finished
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &currentSyncStructures(frame_count).m_render_semaphore;

	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;

	//Sumbit command buffer to queue and execute. m_render_fence will block until commands finish
	VkQueue graphics_queue = RenderModule::getBackend()->getGraphicsQueue();
	VK_CHECK(vkQueueSubmit(graphics_queue, 1, &submit_info, cmd_buffer.fence()));


	//Put image on visible window. Must wait on m_render_semaphore to ensure that drawing commands have finished
	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.pNext = nullptr;

	VkSwapchainKHR swapchain = vk_swapchainKHR();
	present_info.pSwapchains = &swapchain;
	present_info.swapchainCount = 1;

	present_info.pWaitSemaphores = &currentSyncStructures(frame_count).m_render_semaphore;
	present_info.waitSemaphoreCount = 1;
	
	present_info.pImageIndices = &currentImageIndex();


	if (checkSwapchainOutOfDate(vkQueuePresentKHR(graphics_queue, &present_info)))
	{
		RenderModule::getBackend()->rebuildSwapchain();
	}


	//Increment frame counter
	RenderModule::getBackend()->incrementFrameCounter();

	
}


}