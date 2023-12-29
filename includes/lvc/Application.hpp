#pragma once

#include "lvc/DebugUtilsMessenger.hpp"
#include "lvc/Device.hpp"
#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

namespace lvc
{
	class Application
	{
	public:
		Application();

		void run() const;

	private:
		Instance* m_instance;
		DebugUtilsMessenger* m_debug_messenger;
		Window* m_window;
		Device* m_device;

		void mainLoop() const;
		void destroyInstance() const;
	};
}
