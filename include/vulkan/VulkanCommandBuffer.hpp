#pragma once

#include "VulkanEngine.hpp"

void createCommandPool(VulkanEngine* engine);
void createCommandBuffers(VulkanEngine* engine);
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, VulkanEngine* engine);