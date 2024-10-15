#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class LAVA_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(float x, float y)
			: m_width(x)
			, m_height(y) {}

		inline float getWidth() const { return m_width; }
		inline float getHeight() const { return m_height; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_width << ", " << m_height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		float m_width, m_height;
	};

	class LAVA_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
