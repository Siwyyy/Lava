#include "SceneLayer.h"

#include "Lava.h"

#include "Lava/Events/KeyEvent.h"

void SceneLayer::onAttach()
{
	// Tri 1
	const std::vector<Lava::Vertex3Color> vertices = {
		{{-0.53f,-0.3f,-0.3f},{1.f,0.f,0.f}},
		{{0.53f,-0.3f,-0.3f},{0.f,1.f,0.f}},
		{{0.f,0.6f,-0.3f},{0.f,0.f,1.f}},
		{{0.f,0.f,0.6f},{0.0f,0.0f,0.0f}}
	};
	const std::vector<uint32_t> indices = {1,3,0,3,2,0,2,1,0,2,3,1};
	m_objects.push_back(std::make_shared<Lava::BasicBody3D>(vertices, indices, glm::vec3(1, 1, 0)));

	// Tri 2
	const float transform                           = 2.f;
	const std::vector<Lava::Vertex3Color> vertices2 = {
		{{-0.53f,-0.3f + transform,-0.3f},{1.0f,0.0f,0.0f}},
		{{0.53f,-0.3f + transform,-0.3f},{0.0f,1.0f,0.0f}},
		{{0.0f,0.6f + transform,-0.3f},{0.0f,0.0f,1.0f}},
		{{0.0f,0.0f + transform,0.6f},{0.7f,0.7f,0.7f}}
	};
	const std::vector<uint32_t> indices2 = {1,3,0,3,2,0,2,1,0,2,3,1};
	m_objects.push_back(std::make_shared<Lava::BasicBody3D>(vertices2, indices2));

	const std::vector<Lava::Vertex3Color> x_vertices = {
		{{0.1f,0.1f,0.1f},{0.0f,1.0f,0.0f}},
		{{0.1f,-0.1f,0.1f},{0.0f,1.0f,0.0f}},
		{{0.1f,-0.1f,-0.1f},{0.0f,1.0f,0.0f}},
		{{0.1f,0.1f,-0.1f},{0.0f,1.0f,0.0f}},
		{{0.5f,0.0f,-0.0f},{0.0f,1.0f,0.0f}}
	};
	const std::vector<uint32_t> x_indices = {4,0,1,4,1,2,4,2,3,4,3,0,2,1,0,3,2,0};
	m_objects.push_back(std::make_shared<Lava::BasicBody3D>(x_vertices, x_indices));

	const std::vector<Lava::Vertex3Color> y_vertices = {
		{{0.1f,0.1f,0.1f},{1.0f,0.0f,0.0f}},
		{{0.1f,0.1f,-0.1f},{1.0f,0.0f,0.0f}},
		{{-0.1f,0.1f,-0.1f},{1.0f,0.0f,0.0f}},
		{{-0.1f,0.1f,0.1f},{1.0f,0.0f,0.0f}},
		{{-0.0f,0.5f,0.0f},{1.0f,0.0f,0.0f}}
	};
	const std::vector<uint32_t> y_indices = {4,0,1,4,1,2,4,2,3,4,3,0,2,1,0,3,2,0};
	m_objects.push_back(std::make_shared<Lava::BasicBody3D>(y_vertices, y_indices));

	const std::vector<Lava::Vertex3Color> z_vertices = {
		{{0.1f,0.1f,0.1f},{0.0f,0.0f,1.0f}},
		{{-0.1f,0.1f,0.1f},{0.0f,0.0f,1.0f}},
		{{-0.1f,-0.1f,0.1f},{0.0f,0.0f,1.0f}},
		{{0.1f,-0.1f,0.1f},{0.0f,0.0f,1.0f}},
		{{0.0f,-0.0f,0.5f},{0.0f,0.0f,1.0f}}
	};
	const std::vector<uint32_t> z_indices = {4,0,1,4,1,2,4,2,3,4,3,0,2,1,0,3,2,0};
	m_objects.push_back(std::make_shared<Lava::BasicBody3D>(z_vertices, z_indices));

	// Camera
	m_camera = std::make_shared<Lava::Camera3D>();

	// Pipeline
	m_pipeline = std::make_shared<Lava::Pipeline>();
	m_pipeline->setCamera(m_camera);
	Lava::Application::getInstance().getWindow().getContext()->pushPipeline(m_pipeline);
	m_pipeline->pushObjects(m_objects);
}

void SceneLayer::onDetach() {}

void SceneLayer::onUpdate() { m_camera->update(); }

void SceneLayer::onEvent(Lava::Event& event_)
{
	Lava::EventDispatcher dispatcher(event_);
	/*dispatcher.dispatch<Lava::MouseButtonPressedEvent>([this](auto&& e_) { return onMouseButtonPressed(e_); });*/
	dispatcher.dispatch<Lava::KeyPressedEvent>([this](auto&& e_) { return onKeyPressed(e_); });
}

bool SceneLayer::onMouseButtonPressed(const Lava::MouseButtonPressedEvent& event_) const
{
	switch (event_.getButtonCode())
	{
	default:
		return false;
	}
}

bool SceneLayer::onKeyPressed(const Lava::KeyPressedEvent& event_) const
{
	switch (event_.getKeyCode())
	{
	case LAVA_KEY_ESCAPE:
		Lava::Application::getInstance().shutdown();
		return true; 
	}
}
