#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/util.hpp>

#include "vulkan/VulkanEngine.hpp"


fastgltf::Asset LoadGLBFile(const std::string &filepath,
                 std::vector<Vertex> &outVertices,
                 std::vector<Triangle> &outTriangles);

void processNode(fastgltf::Scene *scene, int const gltfNodeIndex);
