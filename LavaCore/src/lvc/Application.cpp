#include "lvc/Application.hpp"

#include "lvc/DebugMessenger.hpp"
#include "lvc/Device.hpp"
#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <iostream>

using namespace lvc;

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

Application::Application()
	: m_instance(new Instance("LavaCore", "No Engine"))
	, m_debug_messenger(new DebugMessenger(m_instance))
	, m_window(new Window(WIDTH, HEIGHT, "LavaCore - Test", m_instance))
	, m_device(new Device(m_instance, m_window)) {}

void Application::run() const
{
	mainLoop();
	destroyInstance();
}

void Application::mainLoop() const
{
	while (!glfwWindowShouldClose(m_window->handle()))
		glfwPollEvents();
}

void Application::destroyInstance() const
{
	std::clog << "\n--- --- --- --- --- --- --- --- --- ---\n";
	delete m_debug_messenger;
	delete m_window;
	delete m_device;
	delete m_instance;
	std::clog << "--- --- --- --- --- --- --- --- --- ---\n\n";
}
