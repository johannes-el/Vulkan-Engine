#pragma once

#include "VulkanEngine.hpp"

void createGraphicsPipeline(VulkanEngine* engine);
VkShaderModule createShaderModule(const std::vector<char>& code, VulkanEngine* engine);
static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();