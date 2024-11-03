#include "Lava/Lavapch.h"
#include "Lava/Application.h"

#include "Lava/Log.h"
#include "Lava/Resources.h"

namespace Lava
{
	Application* Application::s_instance           = nullptr;
	std::filesystem::path Resources::m_shaders_dir = "Shaders/Dir/Not/Set";

	void Application::init()
	{
		LAVA_ASSERT(!s_instance, "Application instance already running")
		s_instance = this;

		initResources();

		m_window = std::make_unique<Window>(WindowProps("Lava Engine - Test", 720, 480));
		m_window->setEventCallback([this](auto&& e_) { onEvent(e_); });

		initApp();
	}

	void Application::run()
	{
		while (m_running)
		{
			for (Layer* layer : m_layer_stack)
				layer->onUpdate();

			m_window->onUpdate();
		}
	}

	void Application::onEvent(Event& event_)
	{
		EventDispatcher dispatcher(event_);
		dispatcher.dispatch<WindowCloseEvent>([this](auto&& e_) { return onWindowClose(e_); });

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
		layer_->onAttach();
	}

	void Application::pushOverlay(Layer* overlay_)
	{
		m_layer_stack.pushOverlay(overlay_);
		overlay_->onAttach();
	}

	bool Application::onWindowClose(WindowCloseEvent& event_)
	{
		m_running = false;
		return true;
	}
}
