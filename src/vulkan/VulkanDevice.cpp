#include "VulkanDevice.hpp"
#include "VulkanEngine.hpp"
#include "VulkanSwapchain.hpp"
#include <stdexcept>
#include <vector>
#include <set>


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
};

void pickPhysicalDevice(VulkanEngine* engine)
{
	uint32_t count = 0;
	vkEnumeratePhysicalDevices(engine->_vk.instance, &count, nullptr);
	if (count == 0) throw std::runtime_error("No Vulkan GPU found");
	std::vector<VkPhysicalDevice> devices(count);
	vkEnumeratePhysicalDevices(engine->_vk.instance, &count, devices.data());

	for (auto dev : devices) {
		if (isDeviceSuitable(dev, engine)) {
			engine->_vk.physicalDevice = dev;
			return;
		}
	}

	throw std::runtime_error("No suitable GPU found");
}

void createLogicalDevice(VulkanEngine* engine)
{
	QueueFamilyIndices indices = findQueueFamilies(engine->_vk.physicalDevice, engine);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (bEnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(engine->_vk.physicalDevice, &createInfo, nullptr, &engine->_vk.device) != VK_SUCCESS)
		throw std::runtime_error("Failed to create logical device");

	vkGetDeviceQueue(engine->_vk.device, indices.graphicsFamily.value(), 0, &engine->_vk.graphicsQueue);
	vkGetDeviceQueue(engine->_vk.device, indices.presentFamily.value(), 0, &engine->_vk.presentQueue);
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VulkanEngine* engine)
{
	QueueFamilyIndices indices;
	uint32_t count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
	std::vector<VkQueueFamilyProperties> families(count);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &count, families.data());

	for (uint32_t i = 0; i < count; ++i) {
		if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphicsFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, engine->_vk.surface, &presentSupport);

		if (presentSupport) {
			indices.presentFamily = i;
		}
	}

	return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
					nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
					availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(),
						deviceExtensions.end());

	for (const auto &extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}
	return requiredExtensions.empty();
}

bool isDeviceSuitable(VkPhysicalDevice device, VulkanEngine* engine)
{
	QueueFamilyIndices indices = findQueueFamilies(device, engine);
	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		auto swapChainSupport = querySwapChainSupport(engine, device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}