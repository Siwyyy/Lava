#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>

namespace lava
{
	class Application;

	class Window
	{
	public:
		Window(const int& width,
					 const int& height,
					 const char* title,
					 const VkInstance& instance,
					 void* app);
		Window() = delete;
		~Window();

		inline GLFWwindow& hGlfwWindow() const { return *m_window; }

		inline VkSurfaceKHR& hVkSurface() { return m_surface; }
		inline const VkSurfaceKHR& hVkSurface() const { return m_surface; }

		static void frameBufferResizeCallback(GLFWwindow* t_window, int t_width, int t_height);

	private:
		const VkInstance& m_instance;

		GLFWwindow* m_window;
		VkSurfaceKHR m_surface;
	};
}
