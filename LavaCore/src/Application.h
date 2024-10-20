#pragma once

#include "Events/ApplicationEvent.h"
#include "Graphics/Window.h"

namespace Lava
{
	class LAVA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		void onEvent(Event& event_);

	private:
		bool onWindowClose(WindowCloseEvent& event_);
		bool onWindowResize(WindowResizeEvent& event_);

		std
		::unique_ptr<Window> m_window;

		bool m_running = true;
		static Application* s_instance;
	};

	Application* createApplication();
}
