#pragma once

#include "Veist/Graphics/Vulkan/Images/Image.h"
#include "Veist/Graphics/Vulkan/Commands/CommandBuffer.h"
#include <vulkan/vulkan.h>

namespace Veist
{

struct GPUinfo_t;

class Swapchain
{

public: 

	Swapchain(const VkExtent2D& extent);
	Swapchain(const VkExtent2D& extent, std::shared_ptr<Swapchain> old_swapchain);

	~Swapchain();

	void present(const CommandBuffer& cmd_buffer);//TODO
	void beginNextFrame();



	struct SyncStructures{
		VkSemaphore  m_present_semaphore;
		VkSemaphore  m_render_semaphore;
		VkFence      m_render_fence; //will be used for when swapchain needs rebuilding
	};

	VkSwapchainKHR vk_swapchainKHR() const { return m_swapchain; };
	uint32_t& currentImageIndex() {return m_img_idx; };
	const VkExtent2D& extent() const { return m_extent; }
	uint32_t imageCount() const {return m_images.size();};
	std::vector<std::unique_ptr<SwapchainImage>>& images() {return m_images;};
	SyncStructures& currentSyncStructures(uint32_t frame_count) { return m_sync_structs[frame_count % m_image_count]; };

private:

	void createSwapchain();
	void createSyncStructures();

	VkSwapchainKHR m_swapchain;

	std::shared_ptr<Swapchain> m_old_swapchain;

	std::vector<std::unique_ptr<SwapchainImage>> m_images;
	std::vector<SyncStructures> m_sync_structs;

	ImageFormat                 m_format;
	VkExtent2D					m_extent;
	uint32_t					m_img_idx{0};
	uint32_t					m_image_count;

};


}