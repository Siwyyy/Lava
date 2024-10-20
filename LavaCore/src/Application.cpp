#include "Lavapch.h"
#include "Application.h"

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
		}
	}

	void Application::onEvent(Event& event_)
	{
		EventDispatcher dispatcher(event_);
		dispatcher.dispatch<WindowCloseEvent>([this](auto&& e_) { return onWindowClose(e_); });

		LAVA_CORE_TRACE("Received event: {0}", event_);
	}

	bool Application::onWindowClose(WindowCloseEvent& event_)
	{
		m_running = false;
		return true;
	}
}
