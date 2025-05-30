#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include <optional>
#include <set>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanEngine.hpp"

// --- Instance & Debug ---
void createInstance(VulkanEngine* engine);
bool checkValidationLayerSupport();
std::vector<const char*> getRequiredExtensions();
void setupDebugMessenger(VulkanEngine* engine);
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
VkResult createDebugUtilsMessengerEXT(VulkanEngine* engine, const VkDebugUtilsMessengerCreateInfoEXT*, const VkAllocationCallbacks*, VkDebugUtilsMessengerEXT*);
void DestroyDebugUtilsMessengerEXT(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

// --- Surface ---
void createSurface(VulkanEngine* engine);