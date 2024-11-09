#pragma once

#include "Lava.h"
#include "Lava/Events/KeyEvent.h"

class SceneLayer final : public Lava::Layer
{
public:
	SceneLayer() = default;

	void onAttach() override;

	void onDetach() override;

	void onUpdate() override;

	void onEvent(Lava::Event& event_) override;

private:
	std::shared_ptr<Lava::Pipeline> m_pipeline;
	std::shared_ptr<Lava::Camera3D> m_camera;
	std::vector<std::shared_ptr<Lava::BasicBody3D>> m_objects;

	bool onMouseButtonPressed(const Lava::MouseButtonPressedEvent& event_) const;
	bool onKeyPressed(const Lava::KeyPressedEvent& event_) const;
};
