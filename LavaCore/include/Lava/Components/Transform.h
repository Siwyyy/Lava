#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Lava::Components
{
	class Transform
	{
	public:
		Transform(const glm::vec3& position_ = glm::vec3(0.f), const glm::quat& rotation_ = glm::quat())
		{
			transform.position = position_;
			transform.rotation = rotation_;
		}

		virtual ~Transform() = default;

		inline Transform* getTransform() { return this; }

		inline glm::mat4 getTranslationMatrix() const { return glm::translate(glm::mat4(1.0f), transform.position); }
		inline glm::mat4 getRotationMatrix() const { return glm::toMat4(transform.rotation); }

		struct Data
		{
			glm::vec3 position;
			glm::quat rotation;
		} transform;
	};
}
