#include "Lava.h"

#include "Lava/Renderer/Vertex.h"

class ExampleLayer final : public Lava::Layer
{
public:
	ExampleLayer() = default;

	inline void onAttach() override
	{
		const std::vector<Lava::Vertex3Color> vertices = {
			{{-0.53f,-0.3f,-0.3f},{1.f,0.f,0.f}},
			{{0.53f,-0.3f,-0.3f},{0.f,1.f,0.f}},
			{{0.f,0.6f,-0.3f},{0.f,0.f,1.f}},
			{{0.f,0.f,0.6f},{0.7f,0.7f,0.7f}}
		};

		const std::vector<uint32_t> indices = {1,3,0,3,2,0,2,1,0,2,3,1};

		const float transform = 2.f;

		const std::vector<Lava::Vertex3Color> vertices2 = {
			{{-0.53f,-0.3f + transform,-0.3f},{1.0f,0.0f,0.0f}},
			{{0.53f,-0.3f + transform,-0.3f},{0.0f,1.0f,0.0f}},
			{{0.0f,0.6f + transform,-0.3f},{0.0f,0.0f,1.0f}},
			{{0.0f,0.0f + transform,0.6f},{0.7f,0.7f,0.7f}}
		};

		const std::vector<uint32_t> indices2 = {1,3,0,3,2,0,2,1,0,2,3,1};

		m_pipeline = std::make_shared<Lava::Pipeline>();
		m_objects.push_back(std::make_shared<Lava::BasicBody3D>(vertices, indices));
		m_objects.push_back(std::make_shared<Lava::BasicBody3D>(vertices2, indices2));
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
			return false;

		static uint32_t object_counter = 0;
		if (object_counter < m_objects.size())
			m_pipeline->pushObjects(m_objects[object_counter++]);
		else
			LAVA_CLIENT_DEBUG("All objects are being drawn");

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
