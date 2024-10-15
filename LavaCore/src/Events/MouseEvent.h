#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class LAVA_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y)
			: m_mouse_x(x)
			, m_mouse_y(y) {}

		inline float getX() const { return m_mouse_x; }
		inline float getY() const { return m_mouse_y; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_mouse_x << ", " << m_mouse_y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouse_x, m_mouse_y;
	};
}
