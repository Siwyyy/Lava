#include "Camera.h"

#include "Lava.h"

#include <algorithm>

void Camera::movement()
{
	glm::vec3 direction = {0,0,0};
	if (Lava::Input::isKeyPressed(LAVA_KEY_W)) { direction.y += 1.0f; }
	if (Lava::Input::isKeyPressed(LAVA_KEY_S)) { direction.y += -1.0f; }

	if (Lava::Input::isKeyPressed(LAVA_KEY_A)) { direction.x += -1.0f; }
	if (Lava::Input::isKeyPressed(LAVA_KEY_D)) { direction.x += 1.0f; }

	if (Lava::Input::isKeyPressed(LAVA_KEY_SPACE)) { direction.z += 1.0f; }
	if (Lava::Input::isKeyPressed(LAVA_KEY_LEFT_SHIFT)) { direction.z += -1.0f; }

	if (!(direction.x != 0.f || direction.y != 0.f || direction.z != 0.f))
		return;

	glm::quat q_yaw = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 0, 1));
	direction       = conjugate(q_yaw) * glm::normalize(direction) * m_speed * Lava::Time::deltaTime();
	moveBy(direction);
}

void Camera::rotation()
{
	glm::vec2 new_mouse_pos = Lava::Input::getMousePosition();
	float dz_angle          = (new_mouse_pos.x - m_last_mouse_pos.x) / 15.0f;
	float dx_angle          = (new_mouse_pos.y - m_last_mouse_pos.y) / 15.0f;
	m_last_mouse_pos        = new_mouse_pos;

	// Apply rotation changes
	m_pitch = std::clamp(m_pitch + dx_angle, 0.0f, 180.0f);
	m_yaw += dz_angle;

	// Calculate rotation
	glm::quat x_rotation = glm::angleAxis(glm::radians(m_pitch), glm::vec3(1, 0, 0));
	glm::quat z_rotation = glm::angleAxis(glm::radians(m_yaw), glm::vec3(0, 0, 1));
	setRotation(glm::normalize(x_rotation * z_rotation));
}
