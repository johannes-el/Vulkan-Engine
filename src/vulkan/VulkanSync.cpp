#include "vulkan/VulkanSync.hpp"

void createSyncObjects(VulkanEngine* engine)
{
	size_t MAX_FRAMES_IN_FLIGHT = engine->_vk.swapchainImages.size();

	engine->_vk.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	engine->_vk.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	engine->_vk.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	engine->_vk.imagesInFlight.resize(MAX_FRAMES_IN_FLIGHT, VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(engine->_vk.device, &semaphoreInfo, nullptr, &engine->_vk.imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(engine->_vk.device, &semaphoreInfo, nullptr, &engine->_vk.renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(engine->_vk.device, &fenceInfo, nullptr, &engine->_vk.inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create synchronization objects!");
		}
	}

	engine->_vk.currentFrame = 0;
}

void cleanupSyncObjects(VulkanEngine* engine)
{
	for (auto sem : engine->_vk.imageAvailableSemaphores)
		vkDestroySemaphore(engine->_vk.device, sem, nullptr);
	for (auto sem : engine->_vk.renderFinishedSemaphores)
		vkDestroySemaphore(engine->_vk.device, sem, nullptr);
	for (auto fence : engine->_vk.inFlightFences)
		vkDestroyFence(engine->_vk.device, fence, nullptr);
	engine->_vk.imageAvailableSemaphores.clear();
	engine->_vk.renderFinishedSemaphores.clear();
	engine->_vk.inFlightFences.clear();
	engine->_vk.imagesInFlight.clear();
}
