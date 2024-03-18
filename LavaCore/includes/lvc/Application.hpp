#pragma once
#include <vulkan/vulkan_core.h>

namespace lvc
{
	class CommandBuffer;
}

namespace lvc
{
	class Instance;
	class Swapchain;
	class RenderPass;
	class GraphicsPipeline;
	class CommandPool;

	class Application
	{
	public:
		Application();
		~Application();

		void run();

	private:
		Instance* const m_instance;
		Swapchain* const m_swap_chain;
		RenderPass* const m_render_pass;
		GraphicsPipeline* const m_graphics_pipeline;
		CommandPool* const m_command_pool;
		CommandBuffer* const m_command_buffer;

		VkSemaphore m_semaphore_image_available;
		VkSemaphore m_semaphore_render_finished;
		VkFence m_fence_in_flight;

		void mainLoop() const;
		void createSyncObjects(const VkDevice& t_device);
		void draw(const VkDevice& t_device,
							const VkCommandBuffer& t_command_buffer,
							const VkQueue& t_graphics_queue,
							const VkQueue& t_present_queue,
							const VkSwapchainKHR& t_swapchain) const;
	};
}
