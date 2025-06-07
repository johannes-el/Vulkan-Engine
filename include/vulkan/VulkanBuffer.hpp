#pragma once

#include <vector>
#include "VulkanEngine.hpp"

void createVertexBuffer(std::vector<Vertex> vertices, VulkanEngine* engine);
void createIndexBuffer(std::vector<uint16_t> indices, VulkanEngine* engine);
void createUniformBuffers(VulkanEngine* engine);
void updateUniformBuffer(uint32_t currentImage, VulkanEngine* engine, float scale);
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanEngine* engine);
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VulkanEngine* engine);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VulkanEngine* engine);
