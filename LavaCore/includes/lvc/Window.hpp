#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>

namespace lvc
{
	class Instance;

	class Window
	{
	public:
		Window(const int& width, const int& height, const char* title, Instance* instance);
		Window() = delete;
		~Window();

		inline GLFWwindow* hWindow() const { return m_window; }

		inline VkSurfaceKHR& hSurface() { return m_surface; }
		inline const VkSurfaceKHR& hSurface() const { return m_surface; }

	private:
		GLFWwindow* m_window;
		VkSurfaceKHR m_surface;
		Instance* m_instance;
	};
}
