#pragma once

#include "Lava/GameObject.h"
#include "Lava/Components/Mesh.h"
#include "Lava/Components/Transform.h"

namespace Lava
{
	struct Vertex3Color;
}

class ObjectTest : public Lava::GameObject
{
public:
	ObjectTest(const std::vector<Lava::Vertex3Color>& vertices_,
						 const std::vector<uint32_t>& indices_,
						 const glm::vec3& position_ = glm::vec3(0.f),
						 const glm::quat& rotation_ = glm::quat())
		: GameObject(vertices_, indices_, position_, rotation_) {}


	~ObjectTest() = default;
};
