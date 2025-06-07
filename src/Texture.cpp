#include <stdexcept>
#include "vulkan/VulkanEngine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void createTextureImage()
{
	int texWidth, texHeight, texChannels;

	stbi_uc* pixels = stbi_load("", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
}
