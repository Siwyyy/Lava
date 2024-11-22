#pragma once

#include "Lava/Components/BasicBody3D.h"
#include "Lava/Components/Transform.h"

namespace Lava
{
	struct Vertex3Color;
}

class ObjectTest final : public Lava::Components::Transform,
												 public Lava::Components::BasicBody3D
{
public:
	ObjectTest(const std::vector<Lava::Vertex3Color>& vertices_,
						 const std::vector<uint32_t>& indices_,
						 const glm::vec3& position_ = glm::vec3(0.f),
						 const glm::quat& rotation_ = glm::quat())
		: Transform(position_, rotation_)
		, BasicBody3D(vertices_, indices_)
	{
		transform_data = getTransform();
	}

	~ObjectTest() override = default;
};
