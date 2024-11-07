#pragma once
#include "Lava/Lavapch.h"

#include "Lava/Events/Event.h"
#include "Lava/Renderer/GraphicsContext.h"

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

		Window(const WindowProps& props_);
		~Window();

		void init(const WindowProps& props_);
		void shutdown();

		void onUpdate();
		void onMouseMoved(float angle_);

		inline uint32_t getWidth() const { return m_data.width; }
		inline uint32_t getHeight() const { return m_data.height; }

		inline void* getNativeWindow() const { return m_window; }
		inline std::shared_ptr<GraphicsContext> getContext() { return m_graphics_context; }

		inline void setEventCallback(const EventCallbackFn& callback_) { m_data.EventCallback = callback_; }

	private:
		struct WindowData
		{
			std::string title;
			uint32_t width = 1080, height = 720;

			EventCallbackFn EventCallback;
		};

		WindowData m_data;

		GLFWwindow* m_window;
		std::shared_ptr<GraphicsContext> m_graphics_context;

	private:
		void createGlfwWindow();
	};
}
