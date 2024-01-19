#pragma once

namespace lvc
{
	class Instance;
	class DebugMessenger;
	class Window;
	class Device;
	class Swapchain;
	class RenderPass;
	class GraphicsPipeline;

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
		Swapchain* m_swap_chain;
		RenderPass* m_render_pass;
		GraphicsPipeline* m_graphics_pipeline;

		void mainLoop() const;
		void destroyInstance() const;
	};
}
