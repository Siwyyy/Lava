#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class MouseMovedEvent : public Event
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

	class MouseButtonPressedEvent : public Event
	{
	public:
		MouseButtonPressedEvent(uint32_t button_): m_button_code(button_) {}

		uint32_t getButtonCode() const { return m_button_code; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_button_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		uint32_t m_button_code;
	};

	class MouseButtonReleasedEvent : public Event
	{
	public :
		MouseButtonReleasedEvent(uint32_t button_): m_button_code(button_) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_button_code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

	private:
		uint32_t m_button_code;
	};
}
