#pragma once
#include  "Lava/LavaPCH.h"

namespace Lava
{
	class Pipeline;
}

namespace Lava
{
	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* window_)
			: m_window(window_) { init(); }

		~GraphicsContext() { shutdown(); }

		void init();
		void shutdown();

		void onUpdate();
		void beginDraw();
		void endDraw();

	public:
		inline void pushPipeline(const std::shared_ptr<Pipeline>& pipeline_) { m_pipelines.push_back(pipeline_); }

	public:
		inline auto& getDevice() { return m_device; }
		inline auto& getGpu() { return m_gpu; }
		inline auto& getGpuProps() { return m_gpu_props; }
		inline auto& getRenderPass() { return m_render_pass; }
		inline auto& getFramesCount() { return m_frames_count; }
		inline auto getCurrentFrameIndex() const { return m_current_frame; }
		inline auto& getExtent2D() { return m_extent_2d; }
		inline auto& getGraphicsQueueIndex() { return m_queue_family_indices.graphics.value(); }
		inline auto& getGraphicsQueue() { return m_graphics_queue; }
		inline const auto& getCurrentCommandBuffer() const { return m_command_buffers[m_current_frame]; }

	private:
		void beginRecordVulkanCommandBuffer(const uint32_t& image_index_) const;
		void endRecordVulkanCommandBuffer() const;

	private:
		GLFWwindow* m_window;

		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR surface_capabilities{};
			std::vector<VkSurfaceFormatKHR> surface_formats;
			std::vector<VkPresentModeKHR> present_modes;
		};

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debug;

		// VkDevice
		VkDevice m_device;
		VkPhysicalDevice m_gpu;
		VkPhysicalDeviceProperties m_gpu_props;
		VkSurfaceKHR m_surface;

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;

			bool isComplete() const { return graphics.has_value() && present.has_value(); }
		} m_queue_family_indices;

		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		// VkSwapchain
		VkSwapchainKHR m_swapchain;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_image_views;
		VkExtent2D m_extent_2d;
		VkSurfaceFormatKHR m_surface_format;
		VkPresentModeKHR m_present_mode;
		// VkRenderPass
		VkRenderPass m_render_pass;
		std::vector<VkFramebuffer> m_framebuffers;
		uint32_t m_image_index;

		// VkCommandPool
		VkCommandPool m_command_pool;
		std::vector<VkCommandBuffer> m_command_buffers;

		// Semaphores and Fences
		std::vector<VkSemaphore> m_semaphore_image_available;
		std::vector<VkSemaphore> m_semaphore_render_finished;
		std::vector<VkFence> m_fence_in_flight;

		uint32_t m_frames_count     = 2;
		uint32_t m_current_frame    = 0;
		bool m_frame_buffer_resized = false;

		std::vector<std::shared_ptr<Pipeline>> m_pipelines;

	private:
		void createVulkanInstance();
		void createVulkanDebug();
		void createVulkanDevice();

		void createVulkanSwapchain();
		void destroyVulkanSwapchain();
		void recreateVulkanSwapchain();
		void createVulkanRenderPass();
		void createVulkanFrameBuffers();
		void destroyVulkanFrameBuffers();
		void recreateVulkanFrameBuffers();

		void createVulkanCommandPool();
		void allocateVulkanCommandBuffers();
		void createVulkanSyncObjects();

	private:
		std::vector<const char*> m_available_instance_ext = {};
		std::vector<const char*> m_required_instance_ext  = {VK_KHR_SURFACE_EXTENSION_NAME,
																												VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
																												VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
		const std::vector<const char*> m_required_gpu_ext = {VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
																												 VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	private: // Debug helper functions //
		static bool checkValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity_,
			VkDebugUtilsMessageTypeFlagsEXT msg_type_,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data_,
			void* p_user_data_);
		static VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo();
		void destroyVulkanDebug() const;

		inline static const std::vector<const char*> s_validation_layers = {"VK_LAYER_KHRONOS_validation"};
#ifdef LAVA_DEBUG
		inline static const bool s_validation_layers_enabled = true;
#else
		inline static const bool s_validation_layers_enabled = false;
#endif
	};
}
