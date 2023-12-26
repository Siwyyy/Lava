#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace lvc
{
	class Window
	{
	public:
		Window(const int& width, const int& height, const std::string& title);
		Window() = delete;
		~Window();

		inline GLFWwindow* handle() const { return m_window; }
		
	private:
		GLFWwindow* m_window;
	};
}
