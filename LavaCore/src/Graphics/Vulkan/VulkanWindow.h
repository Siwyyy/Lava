#pragma once
#include "Lavapch.h"

#include "Graphics/Window.h"

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <vulkan/vulkan.h>

namespace Lava
{
	class VulkanWindow final : public Window
	{
	public:
		VulkanWindow(const WindowProps& props_);
		~VulkanWindow() override;

		void init(const WindowProps& props_);
		void shutdown();

		void onUpdate() override;
		void onMouseMoved(uint32_t angle_) override;

		void draw();

		inline uint32_t getWidth() const override { return m_data.width; }
		inline uint32_t getHeight() const override { return m_data.height; }

		inline void setEventCallback(const EventCallbackFn& callback_) override { m_data.EventCallback = callback_; }

	private:
		struct WindowData
		{
			std::string title;
			uint32_t width, height;
			uint32_t max_frames_in_flight = 2;

			EventCallbackFn EventCallback;
		};

		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;

			bool isComplete() const { return graphics.has_value() && present.has_value(); }
		};

		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR surface_capabilities;
			std::vector<VkSurfaceFormatKHR> surface_formats;
			std::vector<VkPresentModeKHR> present_modes;
		};

		WindowData m_data;

		GLFWwindow* m_window;
		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debug;

		// VkDevice
		VkDevice m_device;
		VkPhysicalDevice m_gpu;
		VkSurfaceKHR m_surface;
		QueueFamilyIndices m_queue_family_indices;
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

		// VkPipeline
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipeline_layout;
		VkDescriptorSetLayout m_descriptor_set_layout;
		VkDescriptorPool m_descriptor_pool;
		std::vector<VkDescriptorSet> m_descriptor_sets;
		VkShaderModule m_vert_shader_module;
		VkShaderModule m_frag_shader_module;
		// BUFFERS
		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
		VkBuffer m_index_buffer;
		VkDeviceMemory m_index_buffer_memory;
		std::vector<VkBuffer> m_uniform_buffers;
		std::vector<VkDeviceMemory> m_uniform_buffers_memory;
		std::vector<void*> m_uniform_buffers_mapped;

		// VkCommandPool
		VkCommandPool m_command_pool;
		std::vector<VkCommandBuffer> m_command_buffers;

		// Semaphores and Fences
		std::vector<VkSemaphore> m_semaphore_image_available;
		std::vector<VkSemaphore> m_semaphore_render_finished;
		std::vector<VkFence> m_fence_in_flight;
		uint32_t m_current_frame  = 0;
		bool frame_buffer_resized = false;

		uint32_t m_angle;

	private: // Initial creation functions //
		void createGlfwWindow();
		void createVulkanInstance();
		void createVulkanDebug();
		void createVulkanDevice();
		void createVulkanSwapchain();
		void createVulkanRenderPass();
		void createVulkanDescriptorSetLayout();
		void createVulkanGraphicsPipeline();
		void createVulkanFrameBuffers();
		void createVulkanCommandPool();
		void createVulkanSyncObjects();

	private:
		void destroyVulkanSwapchain();
		void recreateVulkanSwapchain();

		void destroyVulkanFrameBuffers();
		void recreateVulkanFrameBuffers();

		void allocateVulkanCommandBuffers();
		void recordVulkanCommandBuffer(const uint32_t& command_buffer_index_, const uint32_t& image_index_) const;

		void createVulkanBuffer(VkDeviceSize size_,
														VkBufferUsageFlags usage_,
														VkMemoryPropertyFlags props_,
														VkBuffer& buffer_,
														VkDeviceMemory& buffer_memory_);
		void copyVulkanBuffer(VkBuffer src_buffer_,
													VkBuffer dst_buffer_,
													VkDeviceSize size_);
		void createVulkanVertexBuffer();
		void createVulkanIndexBuffer();
		void createVulkanUniformBuffers();
		void updateUniformBuffer(uint32_t current_frame_);
		void createVulkanDescriptorPool();
		void createVulkanDescriptorSets();

	private:
		std::vector<const char*> m_available_instance_ext = {};
		std::vector<const char*> m_required_instance_ext  = {VK_KHR_SURFACE_EXTENSION_NAME,
																												 VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
																												 VK_KHR_DEVICE_GROUP_CREATION_EXTENSION_NAME};
		const std::vector<const char*> m_required_gpu_ext = {VK_KHR_DEVICE_GROUP_EXTENSION_NAME,
																												 VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	private:
		static bool checkValidationLayerSupport();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity_,
			VkDebugUtilsMessageTypeFlagsEXT msg_type_,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data_,
			void* p_user_data_);
		static VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo();
		void destroyVulkanDebug() const;
		std::vector<char> readShaderFile(const std::string& filename_);
		VkShaderModule createShaderModule(const std::vector<char>& code_) const;

		static const bool validation_layers_enabled;
		inline static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};
	};
}
