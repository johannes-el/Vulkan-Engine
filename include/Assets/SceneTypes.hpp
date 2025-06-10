#pragma once

#include <string>

#include "vulkan/VulkanBufferTypes.hpp"
#include "vulkan/VulkanTypes.hpp"

namespace Scene {
	struct Sampler {
		std::string name;
		VkSampler sampler;
	};

	struct Image {
		std::string name;
		uint32_t mipLevels;
		AllocatedImage image;
		VkImageView view;
	};

	struct Texture {
		std::string name;
		size_t sampler;
		size_t image;
	};

	struct Material {
		std::string name;
		glm::vec4 baseColor;
		size_t texture;
		AllocatedBuffer buffer;
		void* bufferMapped;
		VkDescriptorSet descriptorSet;
	};

	struct Scene {
		std::vector<size_t> nodes;
	};

	struct Node {
		std::vector<size_t> children;
		std::optional<size_t> mesh;
		glm::mat4 localTransformationMatrix;
		glm::mat4 globalTransformationMatrix;
		std::vector<AllocatedBuffer> modelBuffers;
		std::vector<void*> modelBuffersMapped;
		std::vector<VkDescriptorSet> modelDescriptorSets;
	};

	struct Primitive {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		AllocatedBuffer vertexBuffer;
		AllocatedBuffer indexBuffer;
		size_t material;
	};

	struct Mesh {
		std::vector<Primitive> primitives;
	};
} // namespace Scene
