#pragma once

#include "Lava/Input/Input.h"
#include "Lava/Input/KeyCodes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Lava
{
	class Camera3D
	{
	public:
		Camera3D(glm::vec3 position_ = {0,0,0}, const glm::mat4& orientation_ = {1.0f})
			: m_position(position_)
			, m_orientation(orientation_) {}

		inline const glm::vec3& getPosition() const { return m_position; }
		inline const glm::mat4& getOrientation() const { return m_orientation; }

		inline void update()
		{
			updatePosition();
			updateOrientation();
		}

	private:
		glm::vec3 m_position;
		glm::mat4 m_orientation;
		float m_pitch;
		float m_yaw;

		inline void updatePosition()
		{
			if (Input::isKeyPressed(LAVA_KEY_W)) { m_position.y -= .001f; }
			if (Input::isKeyPressed(LAVA_KEY_S)) { m_position.y += .001f; }
			if (Input::isKeyPressed(LAVA_KEY_A)) { m_position.x += .001f; }
			if (Input::isKeyPressed(LAVA_KEY_D)) { m_position.x -= .001f; }
			if (Input::isKeyPressed(LAVA_KEY_SPACE)) { m_position.z -= .001f; }
			if (Input::isKeyPressed(LAVA_KEY_LEFT_SHIFT)) { m_position.z += .001f; }
		}

		inline void updateOrientation()
		{
			glm::vec2 new_mouse_pos = Input::getMousePosition();
			m_pitch                 = new_mouse_pos.y;
			m_yaw                   = new_mouse_pos.x;

			glm::quat q_pitch = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
			glm::quat q_yaw   = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 0, 1));

			glm::quat q_orientation = glm::normalize(q_pitch * q_yaw);

			m_orientation = glm::toMat4(q_orientation);
		}
	};
}
