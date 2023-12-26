#pragma once

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"
#include "lvc/DebugUtilsMessenger.hpp"
#include "lvc/Device.hpp"

#include <vector>

namespace lvc
{
	class Application
	{
	public:
		Application();

		void run() const { mainLoop(); }

	private:
		Instance* m_instance;
		DebugUtilsMessenger* m_debug_messenger;
		Window* m_window;
		Device* m_device;

		void mainLoop() const;
		void destroyInstanceComponents() const;
	};
}
