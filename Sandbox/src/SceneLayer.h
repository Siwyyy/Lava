#pragma once

#include "Lava.h"
#include "Lava/Events/KeyEvent.h"

class Triangle;
class Camera;

class SceneLayer final : public Lava::Layer
{
public:
	SceneLayer() = default;

	void onAttach() override;
	void onDetach() override;
	void onUpdate() override;
	void onDraw() override;
	void onEvent(Lava::Event& event_) override;

private:
	std::shared_ptr<Lava::Pipeline> m_pipeline;
	std::shared_ptr<Camera> m_camera;
	std::vector<std::shared_ptr<Triangle>> m_objects;

	bool onKeyPressed(const Lava::KeyPressedEvent& event_) const;

public:
};
