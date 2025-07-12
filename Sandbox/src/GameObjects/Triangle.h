#pragma once

#include "Lava/ObjectSystem/Actor.h"
#include "Lava/ObjectSystem/MeshComponent.h"
#include "Lava/ObjectSystem/TransformComponent.h"

namespace Lava
{
	struct Vertex3Color;
}

class Triangle : public Lava::ObjectSystem::Actor
{
public:
	Triangle(const std::string& name_,
					 const std::vector<Lava::Vertex3Color>& vertices_,
					 const std::vector<uint32_t>& indices_,
					 const glm::vec3& position_ = glm::vec3(0.f),
					 const glm::quat& rotation_ = glm::quat())
		: Actor(name_)
	{
		auto transform = addComponent<Lava::ObjectSystem::TransformComponent>("Transform");
		transform->setPosition(position_);
		transform->setRotation(rotation_);

		auto mesh = addComponent<Lava::ObjectSystem::MeshComponent>("Mesh", vertices_, indices_);
	}

	~Triangle() override = default;

	void initialize() override {};
};
