

#include "Engine/Renderer/Vulkan/RenderBackend.h"



#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include "imgui_impl_glfw.h"



#ifdef NDEBUG
static const bool validation_layers_enabled = false;
#else
static const bool validation_layers_enabled = true;
#endif


static const std::vector<const char*> g_validation_layers = { "VK_LAYER_KHRONOS_validation" };

static const std::vector<const char*> g_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };



/*
=======================================
Validation layer utility functions
=======================================
*/

static bool checkValidationLayerSupport()
{
	//get list of instance layer properties
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> availible_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, availible_layers.data());

	//Check that validation layers required are supported by instance
	for (const char* layer_name : g_validation_layers) {
		bool layer_found = false;

		for (const auto& layer_properties : availible_layers) {
			if (std::strcmp(layer_name, layer_properties.layerName) == 0)
			{
				layer_found = true;
				break;
			}
		}

		if (!layer_found) {
			return false;
		}
	}

	return true;
}


std::vector<const char*> getRequiredExtensions() 
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions;
	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

	if (validation_layers_enabled) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallBack(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* p_userdata) {

	if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) { 
		std::cerr << "Validation layer: " << p_callback_data->pMessage << std::endl;
	}
	
	return VK_FALSE;

}

static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info) 
{
	create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugCallBack;
	create_info.pUserData = nullptr; //could pass a pointer to something here. leaving it empty for now
}

//Create and destroy function for vulkan debug messenger (validation layers)
static VkResult createDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
	const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger) {

	auto funct = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (funct != nullptr) {
		return funct(instance, p_create_info, p_allocator, p_debug_messenger);
	} else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator) {
		auto funct = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (funct != nullptr) {
			funct(instance, debug_messenger, p_allocator);
		}
}


/*
=======================================
Physical device utility functions
=======================================
*/
static bool checkPhysDeviceExtensionSupport(GPUinfo_t& gpu, std::vector<const char*>& device_extensions){

	//populate set with device extensions
	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());

	//remove avaiilible extensions from set
	for (const auto& extension : gpu.extension_properties) {
		required_extensions.erase(extension.extensionName);
	}

	return required_extensions.empty();
}




/*
=======================================
Swapchain creation utility functions
=======================================
*/


VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow *window) {

	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	} else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		extent.width = std::min<uint32_t>(extent.width, capabilities.maxImageExtent.width);
		extent.width = std::max<uint32_t>(extent.width, capabilities.minImageExtent.width);

		extent.height = std::min<uint32_t>(extent.height, capabilities.maxImageExtent.height);
		extent.height = std::max<uint32_t>(extent.height, capabilities.minImageExtent.height);

		return extent;
	}

}

/*
================================
RenderBackend vulkan boilerplate implementation
================================
*/



void RenderBackend::init(GLFWwindow* window)
{

	m_glfw_window = window;
	//Initialize vulkan context (instance devices queues)
	initContext_VK();
	
}




void RenderBackend::initContext_VK()
{

	//Create vulkan intance
	createInstance();

	//Create validaiton layer debug messenger
	setupDebugMessenger();

	//Create surface
	createSurface();

	//Pick best physical device (and queue families)
	choosePhysicalDevice();

	//Create logical device and queues
	createDeviceAndQueues();

	//Create vma allocator
	createVmaAllocator();

	//Initialize swapchain
	createSwapchainAndImages();

	//Create command pool and command buffers
	createCommandPoolAndBuffers();

	//Create the default renderpass
	createDefaultRenderPass();

	//Create the framebufffers
	createFramebuffers();

	//Create descriptor set pool allocator
	createDescriptorAllocator();

	m_isInitialized = true;
}





void RenderBackend::createInstance() 
{
	if (validation_layers_enabled && !checkValidationLayerSupport()) {
		CRITICAL_ERROR_LOG("Validation layers requested but not availible");
	}
	
	//Application info
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "VKProject";
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "No Engine";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;



	//Instance create info
	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;



	for (int i = 0; i < g_device_extensions.size(); ++i) {
		m_device_extensions.push_back(g_device_extensions[i]);
	}

	for (int i = 0; i < g_validation_layers.size(); ++i) {
		m_validation_layers.push_back(g_validation_layers[i]);
	}

	//validation layers
	VkDebugUtilsMessengerCreateInfoEXT debugmsg_create_info = {}; //extra debug messenger used to debug vkCreateInstance and vkDestroyInstance
	if (validation_layers_enabled) {
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		create_info.ppEnabledLayerNames = m_validation_layers.data();

		populateDebugMessengerCreateInfo(debugmsg_create_info);
		create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugmsg_create_info;

	} else {
		create_info.enabledLayerCount = 0;
	}

	auto extensions = getRequiredExtensions();
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	//Create instance
	VK_CHECK(vkCreateInstance(&create_info, nullptr, &m_instance));
	CONSOLE_LOG("Vulkan instance created");

}





void RenderBackend::setupDebugMessenger()
{
	if(!validation_layers_enabled) return;

	VkDebugUtilsMessengerCreateInfoEXT create_info;
	populateDebugMessengerCreateInfo(create_info);

	VK_CHECK(createDebugUtilsMessengerExt(m_instance, &create_info, nullptr, &m_debug_messenger));
}



void RenderBackend::createSurface() 
{
	VkWin32SurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd = glfwGetWin32Window(m_glfw_window);
	create_info.hinstance = GetModuleHandle(nullptr);

	VK_CHECK(vkCreateWin32SurfaceKHR(m_instance, &create_info, nullptr, &m_surface));

}



void RenderBackend::choosePhysicalDevice()
{

	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);

	if (device_count == 0) {
		CRITICAL_ERROR_LOG("No GPUs found with Vulkan support");
	}

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance, &device_count, devices.data());


	std::vector<GPUinfo_t> gpus_list(device_count);

	//Fill gpu infos
	for (uint32_t i = 0; i < device_count; ++i) {
		GPUinfo_t &gpu = gpus_list[i];
		gpu.device = devices[i];

		//Queue family properties
		{
			uint32_t queue_count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &queue_count, nullptr);

			gpu.queue_family_properties.resize(queue_count);
			vkGetPhysicalDeviceQueueFamilyProperties(gpu.device, &queue_count, gpu.queue_family_properties.data());
		}

		//Extension properties
		{
			uint32_t extension_count = 0;
			vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &extension_count, nullptr);

			gpu.extension_properties.resize(extension_count);
			vkEnumerateDeviceExtensionProperties(gpu.device, nullptr, &extension_count, gpu.extension_properties.data());
		}

		//Surface capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu.device, m_surface, &gpu.surface_capabilities);

		//Surface formats
		{
			uint32_t format_count = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &format_count, nullptr);

			gpu.surface_formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(gpu.device, m_surface, &format_count, gpu.surface_formats.data());
		}

		//Present modes
		{
			uint32_t present_mode_count = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &present_mode_count, nullptr);

			gpu.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(gpu.device, m_surface, &present_mode_count, gpu.present_modes.data());
		}
		
		//Memory properties
		vkGetPhysicalDeviceMemoryProperties(gpu.device, &gpu.memory_properties);

		//Device properties
		vkGetPhysicalDeviceProperties(gpu.device, &gpu.properties);

		//Device features
		vkGetPhysicalDeviceFeatures(gpu.device, &gpu.features);
	}

	//Choose best gpu
	for (uint32_t i = 0; i < device_count; ++i) {
		
		GPUinfo_t &gpu = gpus_list[i];

		int graphics_idx = -1;
		int present_idx = -1;


		if (!checkPhysDeviceExtensionSupport(gpu, m_device_extensions)) {
			continue;
		}

		if (!gpu.features.geometryShader) {
			continue;
		}

		if (gpu.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && device_count > 1) {
			continue;
		}

		if (gpu.surface_formats.size() == 0) {
			continue;
		}

		if (gpu.present_modes.size() == 0) {
			continue;
		}




		//Find graphics queue family
		for (int j = 0; j < gpu.queue_family_properties.size(); ++j) {
			VkQueueFamilyProperties &properties = gpu.queue_family_properties[j];

			if (properties.queueCount == 0) {
				continue;
			}

			if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				graphics_idx = j;
				break;
			}

		}

		//Find present queue family
		for (int j = 0; j < gpu.queue_family_properties.size(); ++j) {
			VkQueueFamilyProperties& properties = gpu.queue_family_properties[j];

			if (properties.queueCount == 0) {
				continue;
			}

			VkBool32 supports_present = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(gpu.device, j, m_surface, &supports_present);
			if (supports_present) {
				present_idx = j;
				break;
			}

		}


		//Is gpu good for both graphics and present?
		if (graphics_idx >= 0 && present_idx >= 0) {
			m_graphics_family_idx = graphics_idx;
			m_present_family_idx = present_idx;
			m_gpu_info = gpu;

			return;
		}

	}

	CRITICAL_ERROR_LOG("No suitable GPU was found");
		
}



void RenderBackend::createDeviceAndQueues()
{

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	std::set<uint32_t> unique_queue_families;

	unique_queue_families.insert(m_graphics_family_idx);
	unique_queue_families.insert(m_present_family_idx);

	float queue_priority = 1.0f;
	for (uint32_t queue_family : unique_queue_families) {
		VkDeviceQueueCreateInfo queue_create_info = {};
		queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.queueFamilyIndex = queue_family;
		queue_create_info.queueCount = 1;
		queue_create_info.pQueuePriorities = &queue_priority;

		queue_create_infos.push_back(queue_create_info);
	}

	//Could select more VkPhysicalDeviceFeatures here, for now all false
	VkPhysicalDeviceFeatures device_features = {};


	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.pEnabledFeatures = &device_features;

	create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions.size());
	create_info.ppEnabledExtensionNames = m_device_extensions.data();

	if (validation_layers_enabled) {
		create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
		create_info.ppEnabledLayerNames = m_validation_layers.data();
	}
	else {
		create_info.enabledLayerCount = 0;
	}

	
	VK_CHECK(vkCreateDevice(m_gpu_info.device, &create_info, nullptr, &m_device));

	vkGetDeviceQueue(m_device, m_graphics_family_idx, 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, m_present_family_idx, 0, &m_present_queue);
}




void RenderBackend::createVmaAllocator()
{
	VmaAllocatorCreateInfo allocator_info = {};
	allocator_info.physicalDevice = m_gpu_info.device;
	allocator_info.device = m_device;
	allocator_info.instance = m_instance;
	vmaCreateAllocator(&allocator_info, &m_allocator);
}




void RenderBackend::createSwapchainAndImages()
{

	VkExtent2D extent = chooseSwapExtent(m_gpu_info.surface_capabilities, m_glfw_window);

	m_swapchain = std::make_unique<Swapchain>(extent);

	ImageProperties depth_img_properties{ {extent.width, extent.height}, {VK_FORMAT_D32_SFLOAT} };
	m_swapchain_depth_image = {depth_img_properties};

}


void RenderBackend::createCommandPoolAndBuffers() {


	for (int i = 0; i < FRAME_OVERLAP_COUNT; i++) {

		CommandPool* command_pool = m_command_pools.emplace_back(std::make_shared<CommandPool>()).get();
		m_command_buffers[i] = command_pool->allocateCommandBuffer();
	}

	//Create command pool for upload context (staging buffers etc)
	m_disposable_pool = std::make_shared<CommandPool>();

}



void RenderBackend::createDefaultRenderPass()
{

	ImageProperties color_props = m_swapchain.get()->images()[0].properties();//{ ImageSize{m_swapchain_extent.width,m_swapchain_extent.height}, ImageFormat{m_swapchain_format} };
	ImageProperties depth_props = m_swapchain_depth_image.properties();//{ ImageSize{m_swapchain_extent.width,m_swapchain_extent.height}, ImageFormat{VK_FORMAT_D32_SFLOAT} };


	std::vector<RenderPass::AttachmentProperties> main_color_attachments;
	main_color_attachments.push_back({ color_props, RenderPass::LoadOp::Clear, ImageUsage::SwapchainImage | ImageUsage::ColorAttachment });

	RenderPass::AttachmentProperties main_depth_attachment{ depth_props, RenderPass::LoadOp::Clear, ImageUsage::SwapchainImage | ImageUsage::DepthAttachment };
	m_render_pass = RenderPass{ main_color_attachments, main_depth_attachment };
}



void RenderBackend::createFramebuffers() 
{

	//create a framebuffer for each swapchain image
	const uint32_t swapchain_image_count = m_swapchain.get()->imageCount();
	for (int i = 0; i < swapchain_image_count; i++) 
	{
		std::vector<ImageBase> color_images = { m_swapchain.get()->images()[i] };
		m_framebuffers.push_back({color_images, m_swapchain_depth_image, &m_render_pass});
	}

}





void RenderBackend::createDescriptorAllocator()
{

	m_descriptor_allocator = std::make_unique<DescriptorSetAllocator>();

}



void RenderBackend::pushToDeletionQueue(std::function<void()> function) {

	m_deletion_queue.pushFunction(function);

}

void RenderBackend::pushToSwapchainDeletionQueue(std::function<void()> function) {

	m_swapchain_deletion_queue.pushFunction(function);

}


void RenderBackend::shutdown() {
	
	if(m_isInitialized){
		
		--m_frame_count;
		vkWaitForFences(m_device, 1, &getCurrentFrameCmdBuffer().fence(), true, 1000000000);
		++m_frame_count;

		m_descriptor_allocator->cleanup();

		m_swapchain_deletion_queue.executeDeletionQueue(); //maybe add the rest to the deletion queue

		m_deletion_queue.executeDeletionQueue(); //maybe add the rest to the deletion queue


		vmaDestroyAllocator(m_allocator);

		vkDestroyDevice(m_device, nullptr);
	
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

		if (validation_layers_enabled) {
			destroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
		}

		vkDestroyInstance(m_instance, nullptr);
	}
}




/*
=====================================
RenderBackend ImGUI initialization
=====================================
*/
void RenderBackend::initImGUI()
{

	if (!m_isInitialized)
	{
		CRITICAL_ERROR_LOG("IMGUI init failed: vulkan not properly intialized");
	}
	
	//1: create descriptor pool for IMGUI. from imgui demo
	VkDescriptorPoolSize pool_sizes[] =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
	};

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imguiPool));


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	ImGui::CreateContext();

	//this initializes imgui for SDL
	ImGui_ImplGlfw_InitForVulkan(m_glfw_window, true);

	//this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = m_instance;
	init_info.PhysicalDevice = m_gpu_info.device;
	init_info.Device = m_device;
	init_info.Queue = m_graphics_queue;
	init_info.DescriptorPool = imguiPool;
	init_info.MinImageCount = m_swapchain.get()->imageCount(); 
	init_info.ImageCount = m_swapchain.get()->imageCount();
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	


	ImGui_ImplVulkan_Init(&init_info, m_render_pass.vk_renderpass());

	//execute a gpu command to upload imgui font textures
	CommandBuffer cmd_buffer = createDisposableCmdBuffer();
	ImGui_ImplVulkan_CreateFontsTexture(cmd_buffer.vk_commandBuffer());
	cmd_buffer.immediateSubmit();


	//clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontUploadObjects();

	//add the destroy the imgui created structures
	m_deletion_queue.pushFunction([=]() {
			vkDestroyDescriptorPool(m_device, imguiPool, nullptr);
			ImGui_ImplVulkan_Shutdown();
		});

		
}


/*
============================================
RenderBackend create disposable cmd buffer
============================================
*/

CommandBuffer& RenderBackend::createDisposableCmdBuffer()
{
	return m_disposable_pool.get()->allocateCommandBuffer(true);
}
