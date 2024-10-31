#pragma once

#include "LayerStack.h"

#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"

#include "Window.h"

namespace Lava
{
	class Application
	{
	public:
		Application()          = default;
		virtual ~Application() = default;

		static Application& getInstance() { return *s_instance; }
		inline Window& getWindow() const { return *m_window; }

		void init();
		void run();

		void onEvent(Event& event_);

		void pushLayer(Layer* layer_);
		void pushOverlay(Layer* overlay_);

	private:
		virtual void initResources() = 0;

	private:
		bool onWindowClose(WindowCloseEvent& event_);
		bool onMouseMoved(const MouseMovedEvent& event_) const;

		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layer_stack;

		static Application* s_instance;
	};

	Application* createApplication();
}
