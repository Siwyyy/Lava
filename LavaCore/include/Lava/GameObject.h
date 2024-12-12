#pragma once

namespace Lava
{
	class GameObject
	{
	public:
		GameObject(const std::vector<Lava::Vertex3Color>& vertices_,
							 const std::vector<uint32_t>& indices_,
							 const glm::vec3& position_ = glm::vec3(0.f),
							 const glm::quat& rotation_ = glm::quat())
			: m_transform(std::make_shared<Components::Transform>(position_, rotation_))
			, m_mesh(std::make_shared<Components::Mesh>(m_transform, vertices_, indices_)) {}

		auto isVisible() const { return m_visibility; }
		const auto& getMesh() const { return m_mesh; }

	protected:
		std::shared_ptr<Components::Transform> m_transform;
		std::shared_ptr<Components::Mesh> m_mesh;

		bool m_visibility = true;
	};
}
