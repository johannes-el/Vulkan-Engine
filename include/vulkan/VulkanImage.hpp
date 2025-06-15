#pragma once

#include "VulkanEngine.hpp"

void createImage(uint32_t width, uint32_t height,
		VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage &image,
		VkDeviceMemory &imageMemory, VulkanEngine *engine);

void createImageViews(VulkanEngine* engine);
void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VulkanEngine *engine);

void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VulkanEngine *engine);
