#pragma once

#include <glm.hpp>
#include <vulkan/vulkan_core.h>

namespace Lava
{
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static VkVertexInputBindingDescription getBindingDescription()
		{
			VkVertexInputBindingDescription binding_description;
			binding_description.binding   = 0;
			binding_description.stride    = sizeof(Vertex);
			binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return binding_description;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
		{
			std::array<VkVertexInputAttributeDescription, 2> attribute_descriptions;

			attribute_descriptions[0].binding  = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
			attribute_descriptions[0].offset   = offsetof(Vertex, pos);

			attribute_descriptions[1].binding  = 0;
			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[1].offset   = offsetof(Vertex, color);

			return attribute_descriptions;
		}
	};

	const std::vector<Vertex> VERTICES = {
		{{-0.5f,-0.5f},{0.0f,1.0f,0.0f}},
		{{0.5f,-0.5f},{0.0f,0.0f,1.0f}},
		{{0.5f,0.5f},{0.0f,1.0f,0.0f}},
		{{-0.5f,0.5f},{1.0f,0.0f,0.0f}}
	};

	const std::vector<uint16_t> INDICES = {0,1,2,2,3,0};

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}
