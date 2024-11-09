#pragma once

#include "Lava/Input/Input.h"
#include "Lava/Input/KeyCodes.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Lava
{
	class Camera3D
	{
	public:
		Camera3D(const glm::vec3& position_ = glm::vec3(0.0f), const glm::quat& rotation_ = {1,0,0,0})
			: m_positionxxx(position_)
			, m_rotation(rotation_) {}

		inline glm::mat4 getViewMatrix() const { return glm::toMat4(m_rotation) * translate(glm::mat4(1.0f), m_positionxxx); }

		inline void update()
		{
			updateOrientation();
			updatePosition();
		}

	private:
		glm::vec3 m_positionxxx;
		glm::quat m_rotation;
		glm::vec2 m_last_mouse_pos = {0.0f,0.0f};
		float m_pitch              = 0.0f;
		float m_yaw                = 0.0f;

		inline void updatePosition()
		{
			glm::vec3 move = {0,0,0};
			if (Input::isKeyPressed(LAVA_KEY_W)) { move.y += 1.0f; }
			if (Input::isKeyPressed(LAVA_KEY_S)) { move.y += -1.0f; }

			if (Input::isKeyPressed(LAVA_KEY_A)) { move.x += -1.0f; }
			if (Input::isKeyPressed(LAVA_KEY_D)) { move.x += 1.0f; }

			if (Input::isKeyPressed(LAVA_KEY_SPACE)) { move.z += 1.0f; }
			if (Input::isKeyPressed(LAVA_KEY_LEFT_SHIFT)) { move.z += -1.0f; }

			if (!(move.x || move.y || move.z))
				return;

			glm::quat q_yaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 0, 1));
			m_positionxxx += conjugate(q_yaw) * -normalize(move) * 0.001f;
		}

		inline void updateOrientation()
		{
			// Check mouse movement
			glm::vec2 new_mouse_pos = Input::getMousePosition();
			float dz_angle          = (new_mouse_pos.x - m_last_mouse_pos.x) / 15.0f;
			float dx_angle          = (new_mouse_pos.y - m_last_mouse_pos.y) / 15.0f;
			m_last_mouse_pos        = new_mouse_pos;

			// Apply rotation changes
			m_pitch = std::clamp(m_pitch + dx_angle, 0.0f, 180.0f);
			m_yaw += dz_angle;

			// Calculate orientation
			glm::quat x_rotation = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
			glm::quat z_rotation = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 0, 1));
			m_rotation           = glm::normalize(x_rotation * z_rotation);
		}
	};
}
