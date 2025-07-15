#pragma once

#include "LayerStack.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"

namespace Lava
{
	class Application
	{
	public:
		Application()          = default;
		virtual ~Application() = default;

		static Application& getInstance() { return *s_instance; }
		inline Window& getWindow() const { return *m_window; }
		inline const std::filesystem::path& getAssetsPath() const { return m_assets_path; }

		void init();
		void run();
		void shutdown();

		void onEvent(Event& event_);

		void pushLayer(Layer* layer_);
		void pushOverlay(Layer* overlay_);

	private:
		// virtual void initResources() = 0;
		virtual void initApp() = 0;

	private:
		bool onWindowClose(WindowCloseEvent& event_);

		std::unique_ptr<Window> m_window;
		bool m_running = true;
		LayerStack m_layer_stack;

		std::filesystem::path m_assets_path;
		static Application* s_instance;
	};

	Application* createApplication();
}
