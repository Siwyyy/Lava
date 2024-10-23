#include "Lavapch.h"
#include "Application.h"

#include "Events/MouseEvent.h"

#include "Log/Log.h"

namespace Lava
{
	Application* Application::s_instance = nullptr;

	Application::Application()
	{
		LAVA_ASSERT(!s_instance, "Application instance already running")
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::create({"Lava Engine - Test",720,480}));
		m_window->setEventCallback([this](auto&& e_) { onEvent(e_); });
	}

	Application::~Application() = default;

	void Application::run()
	{
		while (m_running)
		{
			m_window->onUpdate();

			for (Layer* layer : m_layer_stack)
			{
				layer->onUpdate();
			}
		}
	}

	void Application::onEvent(Event& event_)
	{
		EventDispatcher dispatcher(event_);
		dispatcher.dispatch<WindowCloseEvent>([this](auto&& e_) { return onWindowClose(e_); });
		dispatcher.dispatch<MouseMovedEvent>([this](auto&& e_) { return onMouseMoved(e_); });

		LAVA_CORE_TRACE("Received event: {0}", event_);

		for (auto it = m_layer_stack.end(); it != m_layer_stack.begin();)
		{
			(*--it)->onEvent(event_);
			if (event_.handled())
				break;
		}
	}

	void Application::pushLayer(Layer* layer_)
	{
		m_layer_stack.pushLayer(layer_);
	}

	void Application::pushOverlay(Layer* overlay_)
	{
		m_layer_stack.pushOverlay(overlay_);
	}

	bool Application::onWindowClose(WindowCloseEvent& event_)
	{
		m_running = false;
		return true;
	}

	bool Application::onMouseMoved(MouseMovedEvent& event_)
	{
		m_window->onMouseMoved(event_.getX());
		return true;
	}
}
