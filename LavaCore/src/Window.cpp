#include "Lava/Lavapch.h"
#include "Lava/Window.h"

#include "Lava/Log.h"
#include "Lava/Events/ApplicationEvent.h"
#include "Lava/Events/KeyEvent.h"
#include "Lava/Events/MouseEvent.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

using namespace Lava;

namespace
{
	bool l_glfw_initialized = false;
}

Window::Window(const WindowProps& props_)
{
	init(props_);
	m_graphics_context = std::make_unique<GraphicsContext>(m_window);
}

Window::~Window()
{
	shutdown();
}

void Window::init(const WindowProps& props_)
{
	m_data.title  = props_.title;
	m_data.width  = props_.width;
	m_data.height = props_.height;

	createGlfwWindow();
}

void Window::shutdown()
{
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void Window::onUpdate()
{
	m_graphics_context->onUpdate();
}

void Window::createGlfwWindow()
{
	LAVA_CORE_INFO("Creating window \"{0}\"...", m_data.title);

	if (!l_glfw_initialized)
	{
		glfwInit();
		l_glfw_initialized = true;
	}

	// Create GLFW Window ///////////////////////////////////
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, &m_data);

	// Set callbacks ////////////////////////////////////////
	glfwSetWindowSizeCallback(m_window,
														[](GLFWwindow* window_, int width_, int height_)
														{
															WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
															data.width       = width_;
															data.height      = height_;
															WindowResizeEvent event(width_, height_);
															data.EventCallback(event);
														});

	glfwSetWindowCloseCallback(m_window,
														 [](GLFWwindow* window_)
														 {
															 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
															 WindowCloseEvent event;
															 data.EventCallback(event);
														 });

	glfwSetKeyCallback(m_window,
										 [](GLFWwindow* window_, int key_, int scancode_, int action_, int mods_)
										 {
											 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);

											 switch (action_)
											 {
											 case GLFW_PRESS:
												 {
													 KeyPressedEvent event(key_, false);
													 data.EventCallback(event);
													 break;
												 }
											 case GLFW_RELEASE:
												 {
													 KeyReleasedEvent event(key_);
													 data.EventCallback(event);
													 break;
												 }
											 case GLFW_REPEAT:
												 {
													 KeyPressedEvent event(key_, true);
													 data.EventCallback(event);
													 break;
												 }
											 }
										 });

	glfwSetMouseButtonCallback(m_window,
														 [](GLFWwindow* window_, int button_, int action_, int mods_)
														 {
															 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);

															 switch (action_)
															 {
															 case GLFW_PRESS:
																 {
																	 MouseButtonPressedEvent event(button_);
																	 data.EventCallback(event);
																	 break;
																 }
															 case GLFW_RELEASE:
																 {
																	 MouseButtonReleasedEvent event(button_);
																	 data.EventCallback(event);
																	 break;
																 }
															 }
														 });

	glfwSetCursorPosCallback(m_window,
													 [](GLFWwindow* window_, double x_, double y_)
													 {
														 WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window_);
														 MouseMovedEvent event((float)x_, (float)y_);
														 data.EventCallback(event);
													 });

	LAVA_CORE_INFO("Window \"{0}\" created", m_data.title);
}
