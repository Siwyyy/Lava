#pragma once

#include "Event.h"

#include <sstream>

namespace Lava
{
	class LAVA_API KeyEvent : public Event
	{
	public:
		inline int getKeyCode() const { return m_keycode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(int keycode)
			: m_keycode(keycode) {}

		int m_keycode;
	};

	class LAVA_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, bool repeats_)
			: KeyEvent(keycode)
			, m_repeats(repeats_) {}

		inline int getRepeatCount() const { return m_repeats; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_keycode << (m_repeats ? "(repeats)" : "");
			return ss.str();
		}

		

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		bool m_repeats;
	};

	class LAVA_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}
