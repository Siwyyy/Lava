#include "lvc/Application.hpp"

#include "lvc/DebugMessenger.hpp"
#include "lvc/Device.hpp"
#include "lvc/GraphicsPipeline.hpp"
#include "lvc/Instance.hpp"
#include "lvc/RenderPass.hpp"
#include "lvc/Swapchain.hpp"
#include "lvc/Window.hpp"

#include <iostream>

using namespace lvc;

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 600;

Application::Application()
	: m_instance(new Instance("LavaCore", "No Engine"))
	, m_debug_messenger(new DebugMessenger(m_instance))
	, m_window(new Window(WIDTH, HEIGHT, "LavaCore - Test", m_instance))
	, m_device(new Device(m_instance, m_window))
	, m_swap_chain(new Swapchain(m_device, m_window))
	, m_render_pass(new RenderPass(m_device, m_swap_chain))
	, m_graphics_pipeline(new GraphicsPipeline(m_device, m_swap_chain, m_render_pass)) {}

void Application::run() const
{
	mainLoop();
	destroyInstance();
}

void Application::mainLoop() const
{
	while (!glfwWindowShouldClose(m_window->hWindow()))
		glfwPollEvents();
}

void Application::destroyInstance() const
{
	std::clog << "\n--- --- --- --- --- --- --- --- --- ---\n";
	delete m_graphics_pipeline;
	delete m_render_pass;
	delete m_swap_chain;
	delete m_window;
	delete m_device;
	delete m_debug_messenger;
	delete m_instance;
	std::clog << "--- --- --- --- --- --- --- --- --- ---\n\n";
}
