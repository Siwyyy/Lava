#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class LAVA_API WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32_t width_, uint32_t height_)
			: m_width(width_)
			, m_height(height_) {}

		inline uint32_t getWidth() const { return m_width; }
		inline uint32_t getHeight() const { return m_height; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent (" << m_width << "x" << m_height << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowResize)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)

	private:
		uint32_t m_width, m_height;
	};

	class LAVA_API WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() = default;

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "WindowCloseEvent";
			return ss.str();
		}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}
