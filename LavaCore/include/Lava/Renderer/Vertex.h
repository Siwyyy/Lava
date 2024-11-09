#pragma once
#include "Lava/Lavapch.h"

#include "tiny_obj_loader/tiny_obj_loader.h"

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

	inline void loadModel(const std::filesystem::path& path_,
								 glm::vec3 color_,
								 std::vector<Vertex3Color>& vertices_,
								 std::vector<uint32_t>& indices_)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path_.string().c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		for (size_t i = 0; i < attrib.vertices.size();)
		{
			Vertex3Color vertex = {{attrib.vertices[i++],
															attrib.vertices[i++],
															attrib.vertices[i++]},
														 color_};

			vertices_.push_back(vertex);
		}

		for (const auto& shape : shapes)
			for (auto& index : shape.mesh.indices)
				indices_.push_back(index.vertex_index);
	}
}
