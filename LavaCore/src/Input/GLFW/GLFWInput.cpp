#include "Lavapch.h"
#include "GLFWInput.h"

#include "Application.h"

#include <glfw3.h>

using namespace Lava;

Input* Input::s_instance = new GLFWInput();

bool GLFWInput::isKeyPressedImpl(int32_t keycode_) const
{
	auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state  = glfwGetKey(window, keycode_);
	return state == GLFW_PRESS || GLFW_REPEAT;
}

bool GLFWInput::isMouseButtonPressedImpl(int32_t button_) const
{
	auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	auto state  = glfwGetMouseButton(window, button_);
	return state == GLFW_PRESS;
}

glm::vec2 GLFWInput::getMousePositionImpl() const
{
	auto window = static_cast<GLFWwindow*>(Application::getInstance().getWindow().getNativeWindow());
	double x_pos, y_pos;
	glfwGetCursorPos(window, &x_pos, &y_pos);
	return {x_pos,y_pos};
}
