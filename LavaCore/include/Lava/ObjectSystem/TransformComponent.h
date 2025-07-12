#pragma once
#include "Component.h"

namespace Lava::ObjectSystem
{
	class TransformComponent final : public Component
	{
	public:
		TransformComponent(const std::string& component_name_,
											 const glm::vec3& position_ = glm::vec3(0.f),
											 const glm::quat& rotation_ = glm::quat(),
											 const glm::vec3& scale_    = glm::vec3(1.f))
			: Component(component_name_)
			, m_position(position_)
			, m_rotation(rotation_)
			, m_scale(scale_) {}

		void initialize() override {};
		void tick(float delta_time_) override {};

		inline void setPosition(const glm::vec3& position_) { m_position = position_; }
		inline void setRotation(const glm::quat& rotation_) { m_rotation = rotation_; }
		inline void setScale(const glm::vec3& scale_) { m_scale = scale_; }

		void move(const glm::vec3& offset_);
		void rotate(const glm::quat& additional_rotation_);
		void rotateAxis(const glm::vec3& axis_, float angle_degrees_);
		void scale(const glm::vec3& scale_factors_);
		void lookAt(const glm::vec3& target_);

		inline glm::vec3 getPosition() const { return m_position; }
		inline glm::quat getRotation() const { return m_rotation; }
		inline glm::vec3 getScale() const { return m_scale; }

		inline glm::mat4 getTranslationMatrix() const { return glm::translate(glm::mat4(1.0f), m_position); }
		inline glm::mat4 getRotationMatrix() const { return glm::toMat4(m_rotation); }

	private:
		TransformComponent* m_parent = nullptr;
		std::vector<TransformComponent*> m_children;

		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;

		glm::mat4 m_local_matrix;
		glm::mat4 m_world_matrix;
		bool m_transform_dirty = true;
	};
}
