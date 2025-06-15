#pragma once

#include "vulkan/VulkanEngine.hpp"
#include <filesystem>

void createTextureImage(std::filesystem::path path, VulkanEngine *engine);
void createTextureImageView(VulkanEngine *engine);
VkImageView createImageView(VkImage image, VkFormat format, VulkanEngine *engine);
void createTextureSampler(VulkanEngine *engine);
