#pragma once

#include "VulkanEngine.hpp"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const { return graphicsFamily.has_value(); }
};

void pickPhysicalDevice(VulkanEngine* engine);
void createLogicalDevice(VulkanEngine* engine);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VulkanEngine* engine);
bool checkDeviceExtensionSupport(VkPhysicalDevice device);
bool isDeviceSuitable(VkPhysicalDevice device, VulkanEngine* engine);