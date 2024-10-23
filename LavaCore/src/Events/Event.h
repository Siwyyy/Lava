#pragma once
#include "Lavapch.h"

#include "Core.h"

namespace Lava
{
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		EventCategoryNone        = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput       = BIT(1),
		EventCategoryKeyboard    = BIT(2),
		EventCategoryMouse       = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType getStaticType() { return EventType::##type; }\
                               virtual EventType getEventType() const override { return getStaticType(); }\
                               virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

	class LAVA_API Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		inline bool handled() const { return m_handled; }
		inline bool isInCategory(EventCategory category_) const { return getCategoryFlags() & category_; }

	protected:
		bool m_handled = false;
	};

	class LAVA_API EventDispatcher
	{
		template <typename T>
		using EventFunction = std::function<bool(T&)>;

	public:
		EventDispatcher() = delete;
		EventDispatcher(Event& event_): m_event(event_) {}
		~EventDispatcher() = default;

		template <typename T>
		bool dispatch(EventFunction<T> function_)
		{
			if (m_event.getEventType() == T::getStaticType())
			{
				m_event.m_handled = function_(*(T*)&m_event);
				return true;
			}
			return false;
		}

	private:
		Event& m_event;
	};

	// SPDLog need this to know how to convert
	inline std::string format_as(const Event& event_)
	{
		return event_.toString();
	}
}
