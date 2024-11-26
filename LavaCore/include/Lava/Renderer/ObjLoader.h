#pragma once

#include "Vertex.h"

#include <tiny_obj_loader/tiny_obj_loader.h>

namespace Lava
{
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
