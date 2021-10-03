#pragma once

#include "Engine/Renderer/Vulkan/Images/Image.h"
#include <vulkan/vulkan.h>

struct GPUinfo_t;

class Swapchain
{

public: 

	Swapchain(const VkExtent2D& extent);

	void present(/*CmdBuffer cmd, Queue VkQueue*/){};//TODO
	

	struct SyncStructures{
		VkSemaphore  m_present_semaphore;
		VkSemaphore  m_render_semaphore;
		VkFence      m_render_fence;
	};

	
	VkSwapchainKHR swapchainKHR() const { return m_swapchain; };
	uint32_t& currentImageIndex() {return m_img_idx; };
	const VkExtent2D& extent() const { return m_extent; }
	uint32_t imageCount() const {return m_images.size();};

	const std::vector<SwapchainImage>& images() const {return m_images;};
	
	SyncStructures& currentSyncStructures(uint32_t frame_count) { return m_sync_structs[frame_count % m_image_count]; };

private:

	void createSwapchain();
	void createSyncStructures();

	VkSwapchainKHR m_swapchain;
	std::vector<SwapchainImage> m_images;
	std::vector<SyncStructures> m_sync_structs;
	ImageFormat                 m_format;
	VkExtent2D					m_extent;
	uint32_t					m_img_idx{0};
	uint32_t					m_image_count;

};

