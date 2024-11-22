#pragma once

#include "Lava/Renderer/RenderObjects/Camera3D.h"

class Camera final : public Lava::Camera3D
{
public:
	Camera(const glm::vec3& position_ = glm::vec3(0.0f)) : Camera3D(position_) {}
	~Camera() override = default;

	inline void updateTransform()
	{
		movement();
		rotation();
	}

private:
	glm::vec2 m_last_mouse_pos = {0.0f,0.0f};
	float m_pitch              = 110.0f;
	float m_yaw                = 0.0f;

	void movement();
	void rotation();
};
