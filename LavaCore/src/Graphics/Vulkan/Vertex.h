#pragma once

#include <glm.hpp>
#include <vulkan/vulkan_core.h>

namespace Lava
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	const std::vector<Vertex> VERTICES = {
		{{-0.5f,-0.5f},{1.0f,0.0f,0.0f}},
		{{0.5f,-0.5f},{0.0f,1.0f,0.0f}},
		{{0.5f,0.5f},{0.0f,0.0f,1.0f}},
		{{-0.5f,0.5f},{1.0f,1.0f,1.0f}}
	};

	const std::vector<uint16_t> INDICES = {0,1,2,2,3,0};
}
