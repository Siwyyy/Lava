#pragma once
#include "Lava/Lavapch.h"
#include "Lava/ObjectSystem/CTransform.h"

namespace Lava::ObjectSystem
{
	class CCamera : public CTransform
	{
	public:
		CCamera(const std::string& actor_name_,
						const glm::vec3& position_ = glm::vec3(0.f)) : CTransform(actor_name_, position_) {}

		void initialize() override { CTransform::initialize(); }
		void tick(float delta_time_) override { CTransform::tick(delta_time_); }

		inline glm::mat4 getViewMatrix() const { return glm::toMat4(getRotation()) * translate(glm::mat4(1.0f), -getPosition()); }
		inline float getFov() const { return m_fov; }

	private:
		float m_fov = 45.f;
	};
}
