#include "Lava.h"

#include "Lava/Resources.h"
#include "Lava/Input/Input.h"
#include "Lava/Input/MouseButtonCodes.h"
#include "Lava/Renderer/BasicBody3DModel.h"
#include "Lava/Renderer/Pipeline.h"

class ExampleLayer final : public Lava::Layer
{
public:
	ExampleLayer() = default;

	inline void onAttach() override
	{
		m_pipeline = std::make_shared<Lava::Pipeline>();
		m_objects.push_back(std::make_shared<Lava::BasicBody3D>(Lava::VERTICES, Lava::INDICES));
		m_objects.push_back(std::make_shared<Lava::BasicBody3D>(Lava::VERTICES2, Lava::INDICES2));
		Lava::Application::getInstance().getWindow().getContext()->pushPipeline(m_pipeline);
	}

	inline void onDetach() override {}
	inline void onUpdate() override {}

	inline void onEvent(Lava::Event& event_) override
	{
		Lava::EventDispatcher dispatcher(event_);
		dispatcher.dispatch<Lava::MouseButtonPressedEvent>([this](auto&& e_) { return onMouseButtonPressed(e_); });
	}

private:
	std::shared_ptr<Lava::Pipeline> m_pipeline;
	std::vector<std::shared_ptr<Lava::BasicBody3D>> m_objects;

	bool onMouseButtonPressed(const Lava::MouseButtonPressedEvent& event_) const
	{
		if (event_.getButtonCode() != LAVA_MOUSE_BUTTON_LEFT)
		{
			m_pipeline->pushObjects(m_objects);
			return false;
		}

		auto mouse_pos = Lava::Input::getMousePosition();

		LAVA_CORE_DEBUG("Mouse position: {0}x{1}", mouse_pos.x, mouse_pos.y);
		return true;
	}
};

class Sandbox final : public Lava::Application
{
public:
	Sandbox()           = default;
	~Sandbox() override = default;

private:
	void initResources() override
	{
		Lava::Resources::setDir(Lava::ResourceDir::Shaders, "./resources/shaders");
	}

	void initApp() override
	{
		pushLayer(new ExampleLayer());
	}
};

Lava::Application* Lava::createApplication()
{
	return new Sandbox;
}
