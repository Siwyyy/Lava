#include "SceneLayer.h"

#include "GameObjects/Camera.h"
#include "GameObjects/ObjectTest.h"

#include "Lava.h"
#include "Lava/Events/KeyEvent.h"
#include "Lava/Input/KeyCodes.h"
#include "Lava/Renderer/ObjLoader.h"

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
	m_objects.push_back(std::make_shared<ObjectTest>(vertices, indices, glm::vec3(1, 1, 0)));

	// Tri 2
	const float transform                           = 2.f;
	const std::vector<Lava::Vertex3Color> vertices2 = {
		{{-0.53f,-0.3f + transform,-0.3f},{1.0f,0.0f,0.0f}},
		{{0.53f,-0.3f + transform,-0.3f},{0.0f,1.0f,0.0f}},
		{{0.0f,0.6f + transform,-0.3f},{0.0f,0.0f,1.0f}},
		{{0.0f,0.0f + transform,0.6f},{0.7f,0.7f,0.7f}}
	};
	const std::vector<uint32_t> indices2 = {1,3,0,3,2,0,2,1,0,2,3,1};
	m_objects.push_back(std::make_shared<ObjectTest>(vertices2, indices2));

	std::vector<Lava::Vertex3Color> y_arrow_vertices{};
	std::vector<uint32_t> y_arrow_indices{};
	Lava::loadModel(Lava::Resources::getDir(Lava::ResourceDir::Models) /= "Arrow.obj", {1.0f,0.0f,0.0f}, y_arrow_vertices, y_arrow_indices);
	m_objects.push_back(std::make_shared<ObjectTest>(y_arrow_vertices, y_arrow_indices));

	std::vector<Lava::Vertex3Color> x_arrow_vertices{};
	std::vector<uint32_t> x_arrow_indices{};
	Lava::loadModel(Lava::Resources::getDir(Lava::ResourceDir::Models) /= "Arrow.obj", {0.0f,1.0f,0.0f}, x_arrow_vertices, x_arrow_indices);
	glm::mat4 x_arrow_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.f, 0.f, -1.f));
	m_objects.push_back(std::make_shared<ObjectTest>(x_arrow_vertices, x_arrow_indices, glm::vec3(0.f), x_arrow_rotation));

	std::vector<Lava::Vertex3Color> z_arrow_vertices{};
	std::vector<uint32_t> z_arrow_indices{};
	Lava::loadModel(Lava::Resources::getDir(Lava::ResourceDir::Models) /= "Arrow.obj", {0.0f,0.0f,1.0f}, z_arrow_vertices, z_arrow_indices);
	glm::mat4 z_arrow_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.f, 0.f, 0.f));
	m_objects.push_back(std::make_shared<ObjectTest>(z_arrow_vertices, z_arrow_indices, glm::vec3(0.f), z_arrow_rotation));

	// Camera
	m_camera = std::make_shared<Camera>(glm::vec3(-1.f, -1.f, 1.f));

	// Pipeline
	m_pipeline = std::make_shared<Lava::Pipeline>();
	m_pipeline->setCamera(m_camera);
	for (const auto& object : m_objects)
	{
		m_pipeline->pushMeshes(object->getMesh());
	}
	Lava::Application::getInstance().getWindow().getContext()->pushPipeline(m_pipeline);
}

void SceneLayer::onDetach() {}

void SceneLayer::onUpdate() { m_camera->updateTransform(); }

void SceneLayer::onEvent(Lava::Event& event_)
{
	Lava::EventDispatcher dispatcher(event_);
	dispatcher.dispatch<Lava::KeyPressedEvent>([this](auto&& e_) { return onKeyPressed(e_); });
}

bool SceneLayer::onKeyPressed(const Lava::KeyPressedEvent& event_) const
{
	switch (event_.getKeyCode())
	{
	case LAVA_KEY_ESCAPE:
		Lava::Application::getInstance().shutdown();
		return true;
	}
	return false;
}
