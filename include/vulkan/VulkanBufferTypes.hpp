#pragma once

#include <vulkan/vulkan.h>
#include "VulkanEngine.hpp"

#include "vk_mem_alloc.h"

struct AllocatedBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VmaAllocationInfo allocationInfo{};
};

struct AllocatedImage {
	VkImage image = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VmaAllocationInfo allocationInfo;
};
