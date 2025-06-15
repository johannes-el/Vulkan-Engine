// Image loading (STB Image)
#include "vulkan/VulkanEngine.hpp"
#include <vulkan/vulkan_core.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "vulkan/VulkanBuffer.hpp"
#include "vulkan/VulkanImage.hpp"

#include <stdexcept>
#include <filesystem>
#include <iostream>

VkImageView createImageView(VkImage image, VkFormat format, VulkanEngine *engine)
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;

	if (vkCreateImageView(engine->_vk.device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture image view!");
	}

	return imageView;
}

void createTextureImage(std::filesystem::path imagePath, VulkanEngine *engine)
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(imagePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory,
		engine
	);

	void* data;
	vkMapMemory(engine->_vk.device, stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size_t>(imageSize));
	vkUnmapMemory(engine->_vk.device, stagingBufferMemory);

	stbi_image_free(pixels);

	createImage(texWidth, texHeight,
		VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		engine->_vk.textureImage,
		engine->_vk.textureImageMemory,
		engine
	);

	transitionImageLayout(engine->_vk.textureImage,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			engine
	);

	copyBufferToImage(stagingBuffer, engine->_vk.textureImage,
			static_cast<uint32_t>(texWidth),
			static_cast<uint32_t>(texHeight),
			engine
	);

	transitionImageLayout(engine->_vk.textureImage,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			engine
	);

	vkDestroyBuffer(engine->_vk.device, stagingBuffer, nullptr);
	vkFreeMemory(engine->_vk.device, stagingBufferMemory, nullptr);
}

void createTextureImageView(VulkanEngine *engine)
{
	engine->_vk.textureImageView = createImageView(engine->_vk.textureImage, VK_FORMAT_R8G8B8A8_SRGB, engine);
}


void createTextureSampler(VulkanEngine *engine)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;

	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(engine->_vk.physicalDevice, &properties);

	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;

	samplerInfo.compareEnable = VK_TRUE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(engine->_vk.device, &samplerInfo, nullptr, &engine->_vk.textureSampler)) {
		throw std::runtime_error("Failed to create texture sampler!");
	}
}
