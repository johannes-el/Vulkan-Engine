#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
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

struct Vertex {
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

struct Triangle {
	uint32_t a, b, c;
};

struct UniformBufferObject {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct InstanceData {
	alignas(16) glm::mat4 model;

	static VkVertexInputBindingDescription getInstanceBindingDescription() {
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(InstanceData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getInstanceAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

		for (int i = 0; i < 4; i++) {
			attributeDescriptions[i].binding = 1;
			attributeDescriptions[i].location = 3 + i;
			attributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[i].offset = offsetof(InstanceData, model) + sizeof(glm::vec4) * i;
		}

		return attributeDescriptions;
	}
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
