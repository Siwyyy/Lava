#include "Lava.h"

#include "Events/KeyEvent.h"

#include "Input/Input.h"
#include "Input/MouseButtonCodes.h"

class ExampleLayer final : public Lava::Layer
{
public:
	ExampleLayer() = default;

	inline void onAttach() override {}
	inline void onDetach() override {}
	inline void onUpdate() override {}

	inline void onEvent(Lava::Event& event_) override
	{
		Lava::EventDispatcher dispatcher(event_);
		dispatcher.dispatch<Lava::MouseButtonPressedEvent>([this](auto&& e_) { return onMouseButtonPressed(e_); });
	}

private:
	bool onMouseButtonPressed(const Lava::MouseButtonPressedEvent& event_) const
	{
		if (event_.getButtonCode() != LAVA_MOUSE_BUTTON_LEFT)
			return false;

		auto mouse_pos = Lava::Input::getMousePosition();

		LAVA_CORE_DEBUG("Mouse position: {0}x{1}", mouse_pos.x, mouse_pos.y);
		return true;
	}
};

class Sandbox final : public Lava::Application
{
public:
	Sandbox()
	{
		pushLayer(new ExampleLayer());
	}

	~Sandbox() override = default;
};

Lava::Application* Lava::createApplication()
{
	return new Sandbox;
}
