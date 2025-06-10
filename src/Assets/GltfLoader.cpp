#include "Assets/GltfLoader.hpp"
#include "fastgltf/core.hpp"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

// Image loading (STB Image)
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <stdexcept>

namespace Assets {

	GltfModel::GltfModel() = default;
	GltfModel::~GltfModel() = default;


	bool GltfModel::Load(int const sceneID)
	{
		{
			auto path = std::filesystem::path{m_Filepath};

			// glTF files list their required extensions
			constexpr auto extensions =
				fastgltf::Extensions::KHR_mesh_quantization | fastgltf::Extensions::KHR_materials_emissive_strength |
				fastgltf::Extensions::KHR_lights_punctual | fastgltf::Extensions::KHR_texture_transform;

			constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
				fastgltf::Options::LoadExternalBuffers | fastgltf::Options::GenerateMeshIndices;

			fastgltf::GltfDataBuffer dataBuffer;
			fastgltf::Parser parser(extensions);

			dataBuffer.FromPath(path);

			fastgltf::Expected<fastgltf::Asset> asset = parser.loadGltf(dataBuffer, path.parent_path(), gltfOptions);
			auto assetErrorCode = asset.error();

			if (assetErrorCode != fastgltf::Error::None)
			{
				return Gltf::GLTF_LOAD_FAILURE;
			}
			m_GltfAsset = std::move(asset.get());
		}

		if (!m_GltfAsset.meshes.size())
		{
			return Gltf::GLTF_LOAD_FAILURE;
		}

		if (sceneID > Gltf::GLTF_NOT_USED)
		{
			if ((m_GltfAsset.scenes.size() - 1) < static_cast<size_t>(sceneID))
			{
				return Gltf::GLTF_LOAD_FAILURE;
			}
		}

		// a scene ID was provided
		if (sceneID > Gltf::GLTF_NOT_USED)
		{
			ProcessScene(m_GltfAsset.scenes[sceneID]);
		}
		else // no scene ID was provided --> use all scenes
		{
			for (auto& scene : m_GltfAsset.scenes)
			{
				ProcessScene(scene);
			}
		}
		return Gltf::GLTF_LOAD_SUCCESS;
	}

	void GltfModel::ProcessScene(fastgltf::Scene& scene)
	{
		size_t nodeCount = scene.nodeIndices.size();
		if (!nodeCount)	{
			return;
		}

		for (uint nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex) {
			ProcessNode(&scene, scene.nodeIndices[nodeIndex]);
		}
	}

	void GltfModel::ProcessNode(fastgltf::Scene* scene, int const gltfNodeIndex)
	{

		auto& node = m_GltfAsset.nodes[gltfNodeIndex];
		std::string nodeName(node.name);

		if (node.meshIndex.has_value()) {
			int meshIndex = node.meshIndex.value();
			LoadVertexData(meshIndex);
		}

		size_t childNodeCount = node.children.size();
		for (size_t childNodeIndex = 0; childNodeIndex < childNodeCount; ++childNodeIndex) {
			int gltfChildNodeIndex = node.children[childNodeIndex];
			ProcessNode(scene, gltfChildNodeIndex);
		}
	}

	void GltfModel::LoadVertexData(uint const meshIndex)
	{

		for (const auto& glTFPrimitive : m_GltfAsset.meshes[meshIndex].primitives)
		{

			size_t vertexCount = 0;
			size_t indexCount = 0;

			// Vertices
			{
				const float* positionBuffer = nullptr;

				if (glTFPrimitive.findAttribute("POSITION") != glTFPrimitive.attributes.end())
				{
					// Get the attribute which contains accessorIndex
					const auto& positionAttr = *glTFPrimitive.findAttribute("POSITION");

					// Load vertex position data using accessorIndex
					auto componentType = LoadAccessor<float>(
						m_GltfAsset.accessors[positionAttr.accessorIndex],
						positionBuffer,
						&vertexCount
						);
				}


				// Append data to model's vertex buffer
				uint numVerticesBefore = m_Vertices.size();
				m_Vertices.resize(numVerticesBefore + vertexCount);
				uint vertexIndex = numVerticesBefore;
				for (size_t vertexIterator = 0; vertexIterator < vertexCount; ++vertexIterator)
				{
					Vertex vertex{};
					// position
					auto position = positionBuffer ?
						glm::vec3(positionBuffer[vertexIterator * 3 + 0],
							positionBuffer[vertexIterator * 3 + 1],
							positionBuffer[vertexIterator * 3 + 2])
						: glm::vec3(0.0f);
					vertex.position = glm::vec3(position.x, position.y, position.z);

					m_Vertices[vertexIndex] = vertex;
					++vertexIndex;
				}
			}

			// Indices
			if (glTFPrimitive.indicesAccessor.has_value())
			{
				auto& accessor = m_GltfAsset.accessors[glTFPrimitive.indicesAccessor.value()];
				indexCount = accessor.count;

				// append indices for submesh to global index array
				size_t globalIndicesOffset = m_Indicies.size();
				m_Indicies.resize(m_Indicies.size() + indexCount);
				uint* destination = m_Indicies.data() + globalIndicesOffset;
				fastgltf::iterateAccessorWithIndex<uint>(m_GltfAsset, accessor, [&](uint submeshIndex, size_t iterator)
					{ destination[iterator] = submeshIndex; });
				uint numTriangles = m_Indicies.size() / 3;
				m_Triangles.resize(numTriangles);
				for (uint iterator{0}; Triangle& triangle : m_Triangles)
				{
					triangle.mV[0] = ConvertToFloat3(m_Vertices[m_Indicies[iterator * 3]].position);
					triangle.mV[1] = ConvertToFloat3(m_Vertices[m_Indicies[iterator * 3 + 1]].position);
					triangle.mV[2] = ConvertToFloat3(m_Vertices[m_Indicies[iterator * 3 + 2]].position);
					++iterator;
				}
			}
		}
	}


	Float3 GltfModel::ConvertToFloat3(const glm::vec3& vec) {
		return {vec.x, vec.y, vec.z};
	}
} // namespace Assets
