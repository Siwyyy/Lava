#include "Lavapch.h"
#include "Window.h"

#include "Application.h"
#include "Instance.h"
#include "Log.h"

#include <vulkan/vulkan.h>



using namespace Lava;

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
		LAVA_CORE_ERROR("Unable to create window surface!");
}

Window::~Window()
{
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	glfwTerminate();
	glfwDestroyWindow(m_window);
}

void Window::frameBufferResizeCallback(GLFWwindow* t_window, [[maybe_unused]] int t_width, [[maybe_unused]] int t_height)
{
	const auto app              = reinterpret_cast<Application*>(glfwGetWindowUserPointer(t_window));
	app->frame_buffer_resized = true;
}
