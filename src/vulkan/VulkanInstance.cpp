#include "vulkan/VulkanInstance.hpp"
#include "vulkan/VulkanEngine.hpp"
#include <iostream>

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

void createInstance(VulkanEngine* engine)
{
	if (bEnableValidationLayers && !checkValidationLayerSupport())
		throw std::runtime_error("Validation layers not supported");

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_3;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (bEnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = &debugCreateInfo;
	} else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &engine->_vk.instance) != VK_SUCCESS)
		throw std::runtime_error("Instance creation failed");
}

bool checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	for (const char* name : validationLayers) {
		bool found = false;
		for (const auto& prop : layers)
			if (strcmp(name, prop.layerName) == 0)
				found = true;
		if (!found)
			return false;
	}
	return true;
}

std::vector<const char*> getRequiredExtensions()
{
	uint32_t count;
	const char** glfwExt = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*> exts(glfwExt, glfwExt + count);
	if (bEnableValidationLayers)
		exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	return exts;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& info)
{
	info = {};
	info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	info.messageSeverity =
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	info.messageType =
		VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	info.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT,
				VkDebugUtilsMessageTypeFlagsEXT,
				const VkDebugUtilsMessengerCallbackDataEXT* pData,
				void*) -> VkBool32 {
		std::cerr << "Validation: " << pData->pMessage << std::endl;
		return VK_FALSE;
	};
}

void setupDebugMessenger(VulkanEngine* engine)
{
	if (!bEnableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT info;
	populateDebugMessengerCreateInfo(info);
	if (createDebugUtilsMessengerEXT(engine, &info, nullptr, &engine->_vk.debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("Debug Messenger creation failed");
}

VkResult createDebugUtilsMessengerEXT(VulkanEngine* engine, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
				const VkAllocationCallbacks* pAllocator,
				VkDebugUtilsMessengerEXT* pMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(engine->_vk.instance, "vkCreateDebugUtilsMessengerEXT");
	return func ? func(engine->_vk.instance, pCreateInfo, pAllocator, pMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func) func(instance, messenger, pAllocator);
}

void createSurface(VulkanEngine* engine)
{
	if (glfwCreateWindowSurface(engine->_vk.instance, engine->_window, nullptr, &engine->_vk.surface) !=
		VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface!");
	}
}
