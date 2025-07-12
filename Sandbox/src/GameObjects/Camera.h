#pragma once

#include "Lava/Renderer/RenderObjects/Camera3D.h"

class Camera final : public Lava::ObjectSystem::Camera3D
{
public:
	Camera(const glm::vec3& position_ = glm::vec3(0.0f)) : Camera3D("Cam", position_) {}
	~Camera() override = default;

	inline void updateTransform()
	{
		movement();
		rotation();
	}

	void initialize() override {};

private:
	glm::vec2 m_last_mouse_pos = {0.0f,0.0f};
	float m_pitch              = 110.0f;
	float m_yaw                = 0.0f;
	float m_speed              = 2.f;

	void movement();
	void rotation();
};
