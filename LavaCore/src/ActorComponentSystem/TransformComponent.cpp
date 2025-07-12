#include "Lava/Lavapch.h"
#include "Lava/ObjectSystem/TransformComponent.h"

namespace Lava::ObjectSystem
{
	void TransformComponent::move(const glm::vec3& offset_)
	{
		m_position        = m_position + offset_;
		m_transform_dirty = true;
	}

	void TransformComponent::rotate(const glm::quat& additional_rotation_)
	{
		m_rotation        = m_rotation * additional_rotation_;
		m_transform_dirty = true;
	}

	void TransformComponent::rotateAxis(const glm::vec3& axis_, float angle_degrees_)
	{
		glm::quat additional_rotation = glm::angleAxis(angle_degrees_, axis_);
		m_rotation                    = m_rotation * additional_rotation;
		m_transform_dirty             = true;
	}

	void TransformComponent::scale(const glm::vec3& scale_factors_)
	{
		m_scale.x *= scale_factors_.x;
		m_scale.y *= scale_factors_.y;
		m_scale.z *= scale_factors_.z;
		m_transform_dirty = true;
	}

	// Utility functions
	void TransformComponent::lookAt(const glm::vec3& target_)
	{
		// Create a rotation that looks at the target
		glm::vec3 direction = glm::normalize(target_ - m_position);
		m_rotation          = glm::quatLookAt(direction, glm::vec3(0, 1, 0));
		m_transform_dirty   = true;
	}
}
