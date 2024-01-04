#pragma once

#include "Instance.hpp"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>

namespace lvc
{
	class Window
	{
	public:
		Window(const int& width, const int& height, const char* title, const Instance* instance);
		Window() = delete;
		~Window();

		inline GLFWwindow* handle() const { return m_window; }
		inline VkSurfaceKHR hSurface() const { return m_surface; }

	private:
		GLFWwindow* m_window;
		VkSurfaceKHR m_surface;
		const Instance* m_instance;
	};
}
