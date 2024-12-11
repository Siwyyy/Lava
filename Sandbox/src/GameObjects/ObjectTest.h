#pragma once

#include "Lava/Components/Mesh.h"
#include "Lava/Components/Transform.h"

namespace Lava
{
	struct Vertex3Color;
}

class ObjectTest
{
public:
	ObjectTest(const std::vector<Lava::Vertex3Color>& vertices_,
						 const std::vector<uint32_t>& indices_,
						 const glm::vec3& position_ = glm::vec3(0.f),
						 const glm::quat& rotation_ = glm::quat())
		: m_transform(std::make_shared<Lava::Components::Transform>(position_, rotation_))
		, m_basic_body_3d(std::make_shared<Lava::Components::Mesh>(m_transform, vertices_, indices_)) {}

	const auto& getMesh() const { return m_basic_body_3d; }

	~ObjectTest() = default;

private:
	std::shared_ptr<Lava::Components::Transform> m_transform;
	std::shared_ptr<Lava::Components::Mesh> m_basic_body_3d;
};
