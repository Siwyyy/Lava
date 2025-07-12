#pragma once

#include "Lava/ObjectSystem/AActor.h"
#include "Lava/ObjectSystem/CMesh.h"
#include "Lava/ObjectSystem/CTransform.h"

namespace Lava
{
	struct Vertex3Color;
}

class Triangle : public Lava::ObjectSystem::AActor
{
public:
	Triangle(const std::string& name_,
					 const std::vector<Lava::Vertex3Color>& vertices_,
					 const std::vector<uint32_t>& indices_,
					 const glm::vec3& position_ = glm::vec3(0.f),
					 const glm::quat& rotation_ = glm::quat())
		: AActor(name_)
	{
		auto transform = addComponent<Lava::ObjectSystem::CTransform>("Transform");
		transform->setPosition(position_);
		transform->setRotation(rotation_);

		auto mesh = addComponent<Lava::ObjectSystem::CMesh>("Mesh", vertices_, indices_);
	}

	void initialize() override { AActor::initialize(); }
};
