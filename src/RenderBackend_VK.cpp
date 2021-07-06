#include "RenderBackend_VK.h"

//Check VkResult macro. this can be replaced by error message/crash dump or throw exception
#define VK_CHECK(x)														\
	do																	\
	{																	\
		VkResult err = x;												\
		if (err)														\
		{																\
			std::cout <<"Detected Vulkan error: " << err << std::endl;	\
			abort();													\
		}																\
	} while (0)






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

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {

	for (const auto& format : formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return format;
		}
	}

	//if best not found return the first one in list
	return formats[0];

}



VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& present_modes) {

	for (const auto& present_mode : present_modes) {
		if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return present_mode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}



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
RenderBackend_VK implementation
================================
*/

RenderBackend_VK::RenderBackend_VK(GLFWwindow* window) : m_glfw_window(window) {}




void RenderBackend_VK::init()
{

	//Initialize vulkan context (instance devices queues)
	initContext_VK();
	
}




void RenderBackend_VK::initContext_VK()
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

	//Initialize swapchain
	createSwapchain();
}





void RenderBackend_VK::createInstance() 
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





void RenderBackend_VK::setupDebugMessenger()
{
	if(!validation_layers_enabled) return;

	VkDebugUtilsMessengerCreateInfoEXT create_info;
	populateDebugMessengerCreateInfo(create_info);

	VK_CHECK(createDebugUtilsMessengerExt(m_instance, &create_info, nullptr, &m_debug_messenger));
}



void RenderBackend_VK::createSurface() 
{
	VkWin32SurfaceCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	create_info.hwnd = glfwGetWin32Window(m_glfw_window);
	create_info.hinstance = GetModuleHandle(nullptr);

	VK_CHECK(vkCreateWin32SurfaceKHR(m_instance, &create_info, nullptr, &m_surface));

}



void RenderBackend_VK::choosePhysicalDevice()
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



void RenderBackend_VK::createDeviceAndQueues()
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




void RenderBackend_VK::createSwapchain()
{

	GPUinfo_t &gpu = m_gpu_info;

	VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(gpu.surface_formats);
	VkPresentModeKHR present_mode = chooseSwapPresentMode(gpu.present_modes);
	VkExtent2D extent = chooseSwapExtent(gpu.surface_capabilities, m_glfw_window);

	uint32_t image_count = gpu.surface_capabilities.minImageCount + 1;
	if (gpu.surface_capabilities.maxImageCount > 0 && image_count > gpu.surface_capabilities.maxImageCount) {
		image_count = gpu.surface_capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	create_info.surface = m_surface;
	create_info.minImageCount = image_count;
	create_info.imageFormat = surface_format.format;
	create_info.imageColorSpace = surface_format.colorSpace;
	create_info.imageExtent = extent;
	create_info.imageArrayLayers = 1;
	create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;


	if (m_graphics_family_idx != m_present_family_idx) {
		
		uint32_t queue_family_indeces[] =  {m_graphics_family_idx, m_present_family_idx};
		create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		create_info.queueFamilyIndexCount = 2;
		create_info.pQueueFamilyIndices = queue_family_indeces;

	} else {
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	create_info.preTransform = gpu.surface_capabilities.currentTransform;
	create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	create_info.presentMode = present_mode;
	create_info.clipped = VK_TRUE;
	create_info.oldSwapchain = VK_NULL_HANDLE; //Unused for now. needed to recreate swapchain

	VK_CHECK(vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swapchain));



	//Create swapchain images and imageview

			//TODO 
}




void RenderBackend_VK::cleanup() {
	
	if (validation_layers_enabled) {
		destroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
	}

	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

	vkDestroyDevice(m_device, nullptr);
	
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

	vkDestroyInstance(m_instance, nullptr);

}