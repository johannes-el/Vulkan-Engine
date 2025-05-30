#include "VulkanEngine.hpp"
#include "VulkanBuffer.hpp"
#include <stdexcept>
#include <cstring>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void createVertexBuffer(std::vector<Vertex> vertices, VulkanEngine* engine)
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, engine);

	void* data;
	vkMapMemory(engine->_vk.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(engine->_vk.device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, engine->_vk.vertexBuffer, engine->_vk.vertexBufferMemory, engine);
	copyBuffer(stagingBuffer, engine->_vk.vertexBuffer, bufferSize, engine);

	vkDestroyBuffer(engine->_vk.device, stagingBuffer, nullptr);
	vkFreeMemory(engine->_vk.device, stagingBufferMemory, nullptr);
}

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VulkanEngine* engine)
{
	VkCommandBuffer commandBuffer;
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = engine->_vk.commandPool;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(engine->_vk.device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffer!");
	}

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(engine->_vk.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(engine->_vk.graphicsQueue);

	vkFreeCommandBuffers(engine->_vk.device, engine->_vk.commandPool, 1, &commandBuffer);
}

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory, VulkanEngine* engine)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(engine->_vk.device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(engine->_vk.device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, engine);

	if (vkAllocateMemory(engine->_vk.device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate buffer memory!");
	}

	vkBindBufferMemory(engine->_vk.device, buffer, bufferMemory, 0);
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VulkanEngine* engine)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(engine->_vk.physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("Failed to find suitable memory type!");
}

void createIndexBuffer(std::vector<uint16_t> indices, VulkanEngine* engine)
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, engine);

	void* data;
	vkMapMemory(engine->_vk.device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(engine->_vk.device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, engine->_vk.indexBuffer, engine->_vk.indexBufferMemory, engine);
	copyBuffer(stagingBuffer, engine->_vk.indexBuffer, bufferSize, engine);

	engine->_vk.indexCount = static_cast<uint32_t>(indices.size());

	vkDestroyBuffer(engine->_vk.device, stagingBuffer, nullptr);
	vkFreeMemory(engine->_vk.device, stagingBufferMemory, nullptr);
}

void createUniformBuffers(VulkanEngine* engine)
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	engine->_vk.uniformBuffers.resize(engine->_vk.swapchainImages.size());
	engine->_vk.uniformBuffersMemory.resize(engine->_vk.swapchainImages.size());
	engine->_vk.uniformBuffersMapped.resize(engine->_vk.swapchainImages.size());

	for (size_t i = 0; i < engine->_vk.swapchainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, engine->_vk.uniformBuffers[i], engine->_vk.uniformBuffersMemory[i], engine);

		vkMapMemory(engine->_vk.device, engine->_vk.uniformBuffersMemory[i], 0, bufferSize, 0, &engine->_vk.uniformBuffersMapped[i]);
	}
}

void updateUniformBuffer(uint32_t currentImage, VulkanEngine* engine)
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), engine->_vk.swapchainExtent.width / (float)engine->_vk.swapchainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	memcpy(engine->_vk.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}
