#include "vulkan/VulkanInstance.hpp"
#include "vulkan/VulkanDevice.hpp"
#include "vulkan/VulkanEngine.hpp"
#include "vulkan/VulkanSwapchain.hpp"
#include "vulkan/VulkanImage.hpp"
#include "vulkan/VulkanRenderPass.hpp"
#include "vulkan/VulkanDescriptor.hpp"
#include "vulkan/VulkanPipeline.hpp"
#include "vulkan/VulkanBuffer.hpp"
#include "vulkan/VulkanImGui.hpp"
#include "vulkan/VulkanSync.hpp"
#include "vulkan/VulkanCommandBuffer.hpp"
#include "Assets/GltfLoader.hpp"
#include "Assets/SceneTypes.hpp"

#include "FileIO.hpp"
#include "Logger.hpp"
#include "vk_mem_alloc.h"

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
	Scene::Primitive primitive;
	primitive.vertices = {
		{{-0.5f, -0.5f,  0.5f}, {1,0,0}, {0.0f, 0.0f}},
		{{ 0.5f, -0.5f,  0.5f}, {0,1,0}, {1.0f, 0.0f}},
		{{ 0.5f,  0.5f,  0.5f}, {0,0,1}, {1.0f, 1.0f}},
		{{-0.5f,  0.5f,  0.5f}, {1,1,1}, {0.0f, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {1,0,0}, {1.0f, 0.0f}},
		{{ 0.5f, -0.5f, -0.5f}, {0,1,0}, {0.0f, 0.0f}},
		{{ 0.5f,  0.5f, -0.5f}, {0,0,1}, {0.0f, 1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {1,1,1}, {1.0f, 1.0f}},
	};

	primitive.indices = {
		0, 1, 2, 2, 3, 0,
		1, 5, 6, 6, 2, 1,
		5, 4, 7, 7, 6, 5,
		4, 0, 3, 3, 7, 4,
		3, 2, 6, 6, 7, 3,
		4, 5, 1, 1, 0, 4,
	};

	std::filesystem::path path = "../models/Fox.glb";
	Assets::GltfModel model;
	if (model.Load(0)) {
		createVertexBuffer(model.GetVertices(), this);
		createIndexBuffer(model.GetIndices(), this);
	}

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
	createVertexBuffer(primitive.vertices, this);
	createIndexBuffer(primitive.indices, this);
	createUniformBuffers(this);
	createDescriptorPool(this);
	createDescriptorSets(this);
	createCommandBuffers(this);
	createSyncObjects(this);

	::initImgui(_window, this);
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

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	cleanupSwapchain(this);

	for (size_t i = 0; i < _vk.swapchainImages.size(); i++) {
		vkDestroyBuffer(_vk.device, _vk.uniformBuffers[i], nullptr);
		vkFreeMemory(_vk.device, _vk.uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(_vk.device, _vk.descriptorPool, nullptr);
	vkDestroyDescriptorPool(_vk.device, _vk.imguiDescriptorPool, nullptr);
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
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Control Panel");

	static float cubeScale = 1.0f;
	ImGui::SliderFloat("Cube Scale", &cubeScale, 0.1f, 3.0f);

	ImGui::End();

	ImGui::Render();

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

	updateUniformBuffer(imageIndex, this, cubeScale);

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
