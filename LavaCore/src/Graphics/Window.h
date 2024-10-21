#pragma once
#include "Events/Event.h"

namespace Lava
{
	struct WindowProps
	{
		std::string title;
		uint32_t width, height;

		WindowProps(const std::string& title_ = "Lava Engine",
								uint32_t width_           = 1080,
								uint32_t height_          = 720)
			: title(title_)
			, width(width_)
			, height(height_) {}
	};

	class Window
	{
	public:
		typedef std::function<void(Lava::Event&)> EventCallbackFn;

		virtual ~Window() = default;

		virtual void onUpdate() = 0;
		virtual void onMouseMoved(uint32_t angle_) = 0;

		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;

		virtual void setEventCallback(const EventCallbackFn& callback_) = 0;

		static Window* create(const WindowProps& props_);
	};
}
