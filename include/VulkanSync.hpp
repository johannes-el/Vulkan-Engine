#pragma once

#include "VulkanEngine.hpp"
#include <stdexcept>

void cleanupSyncObjects(VulkanEngine* engine);

void createSyncObjects(VulkanEngine* engine);