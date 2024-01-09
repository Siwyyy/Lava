#pragma once

namespace lvc
{
	class Instance;
	class DebugMessenger;
	class Window;
	class Device;
	class SwapChain;

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
		SwapChain* m_swap_chain;

		void mainLoop() const;
		void destroyInstance() const;
	};
}
