#pragma once

#include "Lava/ObjectSystem/CCamera.h"

class Camera final : public Lava::ObjectSystem::CCamera
{
public:
	Camera(const glm::vec3& position_ = glm::vec3(0.0f)) : CCamera("Cam", position_) {}

	inline void processController()
	{
		movement();
		rotation();
	}

	void initialize() override { CCamera::initialize(); };

private:
	glm::vec2 m_last_mouse_pos = {0.0f,0.0f};
	float m_pitch              = 110.0f;
	float m_yaw                = 0.0f;
	float m_speed              = 2.f;

	void movement();
	void rotation();
};
