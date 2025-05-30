#pragma once

#include "vulkan/VulkanEngine.hpp"
#include <vector>

class Mesh {
    public:
        Mesh();
        ~Mesh();

        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
};
