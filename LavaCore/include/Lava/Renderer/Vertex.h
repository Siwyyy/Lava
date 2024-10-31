#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

namespace Lava
{
	struct Vertex
	{
		glm::vec3 pos;
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
			attribute_descriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[0].offset   = offsetof(Vertex, pos);

			attribute_descriptions[1].binding  = 0;
			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[1].offset   = offsetof(Vertex, color);

			return attribute_descriptions;
		}
	};

	const std::vector<Vertex> VERTICES = {
		{{-0.53f,-0.3f,-0.3f},{1.f,0.f,0.f}},
		{{0.53f,-0.3f,-0.3f},{0.f,1.f,0.f}},
		{{0.f,0.6f,-0.3f},{0.f,0.f,1.f}},
		{{0.f,0.f,0.6f},{0.7f,0.7f,0.7f}}
	};

	const std::vector<uint16_t> INDICES = {1,3,0,3,2,0,2,1,0,2,3,1};

	struct UniformBufferObject
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};
}
