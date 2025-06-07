#include <iostream>
#include <array>
#include <filesystem>
#include <stdexcept>
#include <vector>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/util.hpp>
#include <fastgltf/tools.hpp>

#include "vulkan/VulkanEngine.hpp"

fastgltf::Asset LoadGLBFile(const std::string& filepath)
{
    if (!std::filesystem::exists(filepath)) {
        throw std::runtime_error("File path doesn't exist!");
    }

    constexpr auto extensions =
        fastgltf::Extensions::KHR_mesh_quantization |
        fastgltf::Extensions::KHR_materials_emissive_strength |
        fastgltf::Extensions::KHR_lights_punctual |
        fastgltf::Extensions::KHR_texture_transform;

    constexpr auto gltfOptions =
        fastgltf::Options::DontRequireValidAssetMember |
        fastgltf::Options::AllowDouble |
        fastgltf::Options::LoadExternalBuffers |
        fastgltf::Options::GenerateMeshIndices;

    fastgltf::GltfDataBuffer dataBuffer;
    fastgltf::Parser parser(extensions);

    auto assetResult = parser.loadGltfBinary(dataBuffer, filepath, gltfOptions);
    if (!assetResult) {
        throw std::runtime_error("Failed to load asset!");
    }

    return std::move(assetResult.get());
}

void loadVertexData(const fastgltf::Asset& asset,
                    uint32_t meshIndex,
                    std::vector<Vertex>& outVertices,
                    std::vector<Triangle>& outTriangles)
{
    const auto& mesh = asset.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives) {
        size_t vertexStart = outVertices.size();
    }
}

void processNode(const fastgltf::Asset& asset,
                 uint32_t nodeIndex,
                 std::vector<Vertex>& outVertices,
                 std::vector<Triangle>& outTriangles)
{
    const auto& node = asset.nodes[nodeIndex];

    if (node.meshIndex.has_value()) {
        loadVertexData(asset, node.meshIndex.value(), outVertices, outTriangles);
    }

    for (uint32_t child : node.children) {
        processNode(asset, child, outVertices, outTriangles);
    }
}

void processScene(const fastgltf::Asset& asset,
                  const fastgltf::Scene& scene,
                  std::vector<Vertex>& outVertices,
                  std::vector<Triangle>& outTriangles)
{
    for (uint32_t nodeIndex : scene.nodeIndices) {
        processNode(asset, nodeIndex, outVertices, outTriangles);
    }
}
