#include "Window.hpp"

#include "Application.hpp"
#include "Instance.hpp"

#include <vulkan/vulkan.h>

#include <iostream>

using namespace lvc;

Window::Window(const int& width,
							 const int& height,
							 const char* title,
							 const VkInstance& instance,
							 void* app)
	: m_instance(instance)
	, m_window(nullptr)
	, m_surface(VK_NULL_HANDLE)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	glfwSetWindowUserPointer(m_window,app);

	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
		throw std::runtime_error("err: Unable to create window surface!\n");
}

Window::~Window()
{
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	std::clog << "Successfully destroyed surface\n";

	glfwTerminate();
	glfwDestroyWindow(m_window);
	std::clog << "Successfully destroyed window\n";
}

void Window::frameBufferResizeCallback(GLFWwindow* t_window, [[maybe_unused]] int t_width, [[maybe_unused]] int t_height)
{
	const auto app              = reinterpret_cast<Application*>(glfwGetWindowUserPointer(t_window));
	app->frame_buffer_resized = true;
}
