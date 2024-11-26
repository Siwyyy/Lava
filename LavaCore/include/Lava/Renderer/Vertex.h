#pragma once
#include "Lava/Lavapch.h"

namespace Lava
{
	struct Vertex3
	{
		Vertex3(glm::vec3 pos_)
			: pos(pos_) {}

		glm::vec3 pos;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> binding_description(1);
			binding_description[0].binding   = 0;
			binding_description[0].stride    = sizeof(Vertex3);
			binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return binding_description;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attribute_descriptions(1);

			attribute_descriptions[0].binding  = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[0].offset   = offsetof(Vertex3, pos);

			return attribute_descriptions;
		}
	};

	struct Vertex3Color : Vertex3
	{
		Vertex3Color(glm::vec3 pos_, glm::vec3 color_)
			: Vertex3(pos_) { color = color_; }

		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions()
		{
			std::vector<VkVertexInputBindingDescription> binding_description(1);
			binding_description[0].binding   = 0;
			binding_description[0].stride    = sizeof(Vertex3Color);
			binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return binding_description;
		}

		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions()
		{
			std::vector<VkVertexInputAttributeDescription> attribute_descriptions(2);

			attribute_descriptions[0].binding  = 0;
			attribute_descriptions[0].location = 0;
			attribute_descriptions[0].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[0].offset   = offsetof(Vertex3Color, pos);

			attribute_descriptions[1].binding  = 0;
			attribute_descriptions[1].location = 1;
			attribute_descriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
			attribute_descriptions[1].offset   = offsetof(Vertex3Color, color);

			return attribute_descriptions;
		}
	};
}
