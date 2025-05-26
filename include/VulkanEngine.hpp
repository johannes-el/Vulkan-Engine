#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>
#include <array>
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr uint32_t SCREEN_WIDTH = 800;
constexpr uint32_t SCREEN_HEIGHT = 600;

const bool bEnableValidationLayers = true;

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};


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

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	std::vector<VkDescriptorSet> descriptorSets;
};


class VulkanEngine {
public:
	void run();

	// --- Structs ---
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete() const { return graphicsFamily.has_value(); }
	};

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

	// --- Surface & Device ---
	void createSurface();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice);
	void createLogicalDevice();

	// --- Image Views ---
	void createImageViews();

	// --- Graphics Pipeline ---
	void createGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	// --- Render Pass ---
	void createRenderPass();

	// --- Framebuffers ---
	void createFramebuffers();

	// --- Command Pool ---
	void createCommandPool();
	void createCommandBuffers();

	// --- Command Buffer ---
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

	// --- Draw Frame ---
	void drawFrame();

	// --- Sync Objects ---
	void createSyncObjects();

	// --- Swapchain ---
	void recreateSwapchain();
	void cleanupSwapchain();

	// --- Vertex Buffer ---
	void createVertexBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);


	// --- Index Buffer ---
	void createIndexBuffer();

	// --- Descriptor Set ---
	void createDescriptorSetLayout();

	// --- Uniform Buffer ---
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);

	// --- Descriptor Pool ---
	void createDescriptorPool();
	void createDescriptorSets();
};