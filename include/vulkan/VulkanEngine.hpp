#pragma once

#define GLFW_INCLUDE_VULKAN
#include <vector>
#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#define NOMINMAX
#include <GLFW/glfw3.h>
#include <optional>
#include <array>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const bool bEnableValidationLayers = true;
#include "Assets/SceneTypes.hpp"


struct VulkanContext {
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;
	std::vector<VkImage> swapchainImages;
	std::vector<VkImageView> swapchainImageViews;
	VkFormat swapchainImageFormat;
	VkExtent2D swapchainExtent;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkFramebuffer> swapchainFramebuffers;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	bool framebufferResized = false;
	uint32_t currentFrame = 0;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkBuffer instanceBuffer;
	VkDeviceMemory instanceBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	VkDescriptorPool imguiDescriptorPool;

	std::vector<VkDescriptorSet> descriptorSets;

	uint32_t indexCount = 0;
	uint32_t instanceCount;
};


class VulkanEngine {
public:
	void run();

	// --- Members ---
	GLFWwindow* _window = nullptr;
	VulkanContext _vk;

	void initImgui();

	// --- Main flow ---
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	// --- Callbacks ---
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

	// --- Draw Frame ---
	void drawFrame();
};
