#pragma once

#include <glm.hpp>
#include <vulkan/vulkan_core.h>

#include <array>
#include <vector>

namespace lvc
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
	};

	const std::vector<Vertex> VERTICES =
	{
		{{0.0f,-0.5f},{1.0f,1.0f,1.0f}},
		{{0.5f,0.5f},{1.0f,0.0f,0.0f}},
		{{-0.5f,0.5f},{1.0f,0.0f,0.0f}}
	};
}
