#pragma once

#include "Lava/Components/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Lava
{
	class Camera3D : public Components::Transform
	{
	public:
		Camera3D(const glm::vec3& position_ = glm::vec3(0.0f))
			: Transform(position_) {}

		virtual ~Camera3D() = default;

		inline glm::mat4 getViewMatrix() const { return glm::toMat4(transform.rotation) * translate(glm::mat4(1.0f), -transform.position); }
		inline float getFov() const { return m_fov; }

	protected:
		inline void setPosition(const glm::vec3& position_) { transform.position = position_; }
		inline void setRotation(const glm::quat& rotation_) { transform.rotation = rotation_; }

		inline void moveBy(const glm::vec3& direction_) { transform.position += direction_; }

	private:
		float m_fov = 45.f;
	};
}
