#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lvc
{
	class Window
	{
	public:
		Window(const uint32_t& width, const uint32_t& height, const std::string& title);
		Window() = delete;
		~Window();

		inline GLFWwindow* handle() { return m_window; }
		
	private:
		GLFWwindow* m_window;
	};
}
