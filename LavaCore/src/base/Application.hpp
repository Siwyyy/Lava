#pragma once

#include "CommandBuffer.hpp"
#include "CommandPool.hpp"
#include "Core.hpp"
#include "DebugMessenger.hpp"
#include "Device.hpp"
#include "Gpu.hpp"
#include "GpuManager.hpp"
#include "GraphicsPipeline.hpp"
#include "Instance.hpp"
#include "RenderPass.hpp"
#include "Swapchain.hpp"
#include "Window.hpp"

#include "SyncObjects.hpp"
#include "VertexBuffer.hpp"

#include <vulkan/vulkan_core.h>

constexpr int WINDOW_WIDTH         = 800;
constexpr int WINDOW_HEIGHT        = 600;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;

namespace lava
{
	class LAVA_BUILD_DLL Application
	{
	public:
		 Application();
		virtual ~Application() noexcept = default;

		void run();

		bool frame_buffer_resized = false;

	private:
		Instance m_instance;
		DebugMessenger m_debug_messenger;
		Window m_window;
		GpuManager m_gpu_manager;
		Gpu m_gpu;
		Device m_device;
		Swapchain m_swapchain;
		RenderPass m_render_pass;
		GraphicsPipeline m_graphics_pipeline;
		CommandPool m_command_pool;
		VertexBuffer m_vertex_buffer;
		CommandBuffer m_command_buffer;
		const SyncObjects m_sync_objects;

		uint32_t m_current_frame = 0;

		void mainLoop();

		void draw(const VkDevice& t_device,
							const std::vector<VkCommandBuffer>& t_command_buffers,
							const VkQueue& t_graphics_queue,
							const VkQueue& t_present_queue,
							const VkSwapchainKHR& t_swapchain);
		void recreateSwapchain();
	};

	Application* createApplication();
}
