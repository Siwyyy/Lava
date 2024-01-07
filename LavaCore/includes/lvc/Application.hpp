#pragma once

namespace lvc
{
	class Instance;
	class DebugMessenger;
	class Window;
	class Device;

	class Application
	{
	public:
		Application();

		void run() const;

	private:
		Instance* m_instance;
		DebugMessenger* m_debug_messenger;
		Window* m_window;
		Device* m_device;

		void mainLoop() const;
		void destroyInstance() const;
	};
}
