#pragma once

#include <filesystem>
#include <vector>
#include "vulkan/VulkanEngine.hpp"
#include "Assets/SceneTypes.hpp"

#include <fastgltf/core.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/base64.hpp>

namespace Gltf {
    constexpr int GLTF_NOT_USED = -1;
    constexpr bool GLTF_LOAD_FAILURE = false;
    constexpr bool GLTF_LOAD_SUCCESS = true;
} // namespace Gltf

struct Float3 {
    float x, y, z;
};

namespace Assets {

    struct Triangle {
        Float3 mV[3];
    };

    /**
     * @brief GltfModel class handles loading, parsing, and basic CPU-side
     * processing of glTF/GLB models. It extracts vertex data and scene graph
     * information into its internal CPU-side data structures.
     * This class does NOT directly interact with Vulkan resources.
     */
    class GltfModel {
    public:
        GltfModel();
        ~GltfModel();

        GltfModel(const GltfModel&) = delete;
        GltfModel& operator=(const GltfModel&) = delete;

        /**
         * @brief Loads a glTF or GLB model from the specified filepath and processes
         * the specified scene ID.
         * @param sceneID The index of the scene to load. Use Gltf::GLTF_NOT_USED to load all scenes.
         * @return True if loading and processing is successful, false otherwise.
         */
        bool Load(int const sceneID);

        // Public accessors for the loaded CPU-side data.
        const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
        const std::vector<uint>& GetIndices() const { return m_Indicies; }
        const std::vector<Triangle>& GetTriangles() const { return m_Triangles; }
        const fastgltf::Asset& GetGltfAsset() const { return m_GltfAsset; }
        const std::filesystem::path& GetFilepath() const { return m_Filepath; }


    private:
        std::filesystem::path m_Filepath;
        fastgltf::Asset m_GltfAsset;

        std::vector<Vertex> m_Vertices;
        std::vector<uint> m_Indicies;
        std::vector<Triangle> m_Triangles;

        // Private helper methods for scene graph traversal and data extraction.
        // These are non-static as they implicitly access class members like m_GltfAsset.
        void ProcessScene(fastgltf::Scene& scene);
        void ProcessNode(fastgltf::Scene* scene, int const gltfNodeIndex);
        void LoadVertexData(uint const meshIndex);

	    template<typename T>
	    fastgltf::ComponentType LoadAccessor(const fastgltf::Accessor& accessor,
                                                       const T*& buffer,
                                                       size_t* count)
		    {
			    if (count) *count = accessor.count;

			    if (!accessor.bufferViewIndex.has_value()) {
				    throw std::runtime_error("Accessor has no buffer view");
			    }

			    const auto& bufferView = m_GltfAsset.bufferViews[*accessor.bufferViewIndex];
			    const auto& bufferData = m_GltfAsset.buffers[bufferView.bufferIndex];

			    return std::visit(fastgltf::visitor{
						    [&](const fastgltf::sources::Array& array) {
							    buffer = reinterpret_cast<const T*>(
								    array.bytes.data() + bufferView.byteOffset + accessor.byteOffset
								    );
							    return accessor.componentType;
						    },
							    [&](const auto&) -> fastgltf::ComponentType {
								    throw std::runtime_error("Unsupported buffer source type");
							    }
							    }, bufferData.data);
		    }

	    static Float3 ConvertToFloat3(const glm::vec3& glmVec);
    };
} // namespace Assets
