#pragma once

#include "Input/Input.h"

namespace Lava
{
	class GLFWInput final : public Input
	{
	private:
		bool isKeyPressedImpl(int32_t keycode_) const override;
		bool isMouseButtonPressedImpl(int32_t button_) const override;
		glm::vec2 getMousePositionImpl() const override;
	};
}
