#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class LAVA_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x_, float y_)
			: m_mouse_x(x_)
			, m_mouse_y(y_) {}

		inline float getX() const { return m_mouse_x; }
		inline float getY() const { return m_mouse_y; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: (" << m_mouse_x << "x" << m_mouse_y << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		float m_mouse_x, m_mouse_y;
	};

	class LAVA_API MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(uint32_t button_): m_button(button_) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		uint32_t m_button;
	};

	class LAVA_API MouseButtonReleasedEvent : public Event
	{
	public :
		MouseButtonReleasedEvent(uint32_t button_): m_button(button_) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		uint32_t m_button;
	};
}
