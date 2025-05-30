#include "VulkanInstance.hpp"
#include "VulkanDevice.hpp"
#include "VulkanEngine.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanImage.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanDescriptor.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImGui.hpp"
#include "VulkanSync.hpp"
#include "VulkanCommandBuffer.hpp"

#include "Mesh.hpp"
#include "FileIO.hpp"
#include "Logger.hpp"
#include "vk_mem_alloc.h"
#include "fastgltf/core.hpp"

void VulkanEngine::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

void VulkanEngine::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<VulkanEngine*>(glfwGetWindowUserPointer(window));
	app->_vk.framebufferResized = true;
}

void VulkanEngine::initWindow()
{
	Logger::Info("Initializing window");
	if (!glfwInit())
		throw std::runtime_error("GLFW init failed");

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	_window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Vulkan", nullptr, nullptr);

	if (!_window) {
		throw std::runtime_error("Window creation failed");
	}
	glfwSetWindowUserPointer(_window, this);
	glfwSetWindowSizeCallback(_window, framebufferResizeCallback);
	glfwSetFramebufferSizeCallback(_window, framebufferResizeCallback);
}

void VulkanEngine::initVulkan()
{
	Mesh mesh;

	createInstance(this);
	setupDebugMessenger(this);
	createSurface(this);
	pickPhysicalDevice(this);
	createLogicalDevice(this);
	createSwapchain(this);
	createImageViews(this);
	createRenderPass(this);
	createDescriptorSetLayout(this);
	createGraphicsPipeline(this);
	createFramebuffers(this);
	createCommandPool(this);
	createVertexBuffer(mesh.vertices, this);
	createIndexBuffer(mesh.indices, this);
	createUniformBuffers(this);
	createDescriptorPool(this);
	createDescriptorSets(this);
	createCommandBuffers(this);
	createSyncObjects(this);
}

void VulkanEngine::mainLoop()
{
	while (!glfwWindowShouldClose(_window)) {
		glfwPollEvents();
		drawFrame();
	}
}

void VulkanEngine::cleanup()
{
	vkDeviceWaitIdle(_vk.device);

	cleanupSwapchain(this);

	for (size_t i = 0; i < _vk.swapchainImages.size(); i++) {
		vkDestroyBuffer(_vk.device, _vk.uniformBuffers[i], nullptr);
		vkFreeMemory(_vk.device, _vk.uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(_vk.device, _vk.descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(_vk.device, _vk.descriptorSetLayout, nullptr);

	vkDestroyBuffer(_vk.device, _vk.vertexBuffer, nullptr);
	vkFreeMemory(_vk.device, _vk.vertexBufferMemory, nullptr);

	vkDestroyBuffer(_vk.device, _vk.indexBuffer, nullptr);
	vkFreeMemory(_vk.device, _vk.indexBufferMemory, nullptr);

	vkDestroyPipeline(_vk.device, _vk.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(_vk.device, _vk.pipelineLayout, nullptr);
	vkDestroyRenderPass(_vk.device, _vk.renderPass, nullptr);

	for (size_t i = 0; i < _vk.imageAvailableSemaphores.size(); i++) {
		vkDestroySemaphore(_vk.device, _vk.imageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(_vk.device, _vk.renderFinishedSemaphores[i], nullptr);
		vkDestroyFence(_vk.device, _vk.inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(_vk.device, _vk.commandPool, nullptr);

	vkDestroyDevice(_vk.device, nullptr);

	if (bEnableValidationLayers)
		DestroyDebugUtilsMessengerEXT(_vk.instance, _vk.debugMessenger, nullptr);

	vkDestroySurfaceKHR(_vk.instance, _vk.surface, nullptr);
	vkDestroyInstance(_vk.instance, nullptr);
	glfwDestroyWindow(_window);
	glfwTerminate();
}



void VulkanEngine::drawFrame()
{
    vkWaitForFences(_vk.device, 1, &_vk.inFlightFences[_vk.currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(
        _vk.device, _vk.swapchain, UINT64_MAX,
        _vk.imageAvailableSemaphores[_vk.currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        vkDeviceWaitIdle(_vk.device);
        recreateSwapchain(this);
        return;
    }

    if (_vk.imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(_vk.device, 1, &_vk.imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    _vk.imagesInFlight[imageIndex] = _vk.inFlightFences[_vk.currentFrame];

    vkResetFences(_vk.device, 1, &_vk.inFlightFences[_vk.currentFrame]);
    vkResetCommandBuffer(_vk.commandBuffers[_vk.currentFrame], 0);

    recordCommandBuffer(_vk.commandBuffers[_vk.currentFrame], imageIndex, this);

	updateUniformBuffer(imageIndex, this);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { _vk.imageAvailableSemaphores[_vk.currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_vk.commandBuffers[_vk.currentFrame];

    VkSemaphore signalSemaphores[] = { _vk.renderFinishedSemaphores[imageIndex] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_vk.graphicsQueue, 1, &submitInfo, _vk.inFlightFences[_vk.currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _vk.swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_vk.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _vk.framebufferResized) {
        _vk.framebufferResized = false;
        vkDeviceWaitIdle(_vk.device);
        recreateSwapchain(this);
    }

    _vk.currentFrame = (_vk.currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}