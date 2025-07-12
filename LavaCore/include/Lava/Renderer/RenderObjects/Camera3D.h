#pragma once
#include "Lava/Lavapch.h"
#include "Lava/ObjectSystem/Actor.h"
#include "Lava/ObjectSystem/TransformComponent.h"

namespace Lava::ObjectSystem
{
	class Camera3D : public Actor
	{
	public:
		Camera3D(const std::string& actor_name_,
						 const glm::vec3& position_ = glm::vec3(0.f)) : Actor(actor_name_)
		{
			addComponent<TransformComponent>("CameraTransform", position_);
		}

		inline glm::mat4 getViewMatrix() const { return glm::toMat4(getComponent<TransformComponent>()->getRotation()) * translate(glm::mat4(1.0f), -getComponent<TransformComponent>()->getPosition()); }
		inline float getFov() const { return m_fov; }

	protected:
		inline void setPosition(const glm::vec3& position_) const { getComponent<TransformComponent>()->setPosition(position_); }
		inline void setRotation(const glm::quat& rotation_) const { getComponent<TransformComponent>()->setRotation(rotation_); }

		inline void moveBy(const glm::vec3& direction_) const { getComponent<TransformComponent>()->move(direction_); }

	private:
		float m_fov = 45.f;
	};
}
