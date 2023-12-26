#include "lvc/Application.hpp"

using namespace lvc;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Application::Application() : m_instance(new Instance("LavaCore", "No Engine"))
                           , m_debug_messenger(new DebugUtilsMessenger(m_instance))
                           , m_window(new Window(WIDTH, HEIGHT, "LavaCore - Test"))
                           , m_device(new Device(m_instance, m_window, Instance::device_extensions)) {}

void Application::mainLoop() const { while (!glfwWindowShouldClose(m_window->handle())) glfwPollEvents(); }

void Application::destroyInstanceComponents() const
{
	delete m_debug_messenger;
	delete m_window;
	delete m_device;
}
