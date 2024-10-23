#pragma once

#include "LayerStack.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
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

		void pushLayer(Layer* layer_);
		void pushOverlay(Layer* overlay_);

	private:
		bool onWindowClose(WindowCloseEvent& event_);
		bool onMouseMoved(MouseMovedEvent& event_);

		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layer_stack;

		static Application* s_instance;
	};

	Application* createApplication();
}
