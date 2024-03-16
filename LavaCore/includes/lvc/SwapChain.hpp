#pragma once

#include <glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	struct QueueFamilyIndices;
}

namespace lvc
{
	class Device;
	class Window;

	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class Swapchain
	{
	public:
		Swapchain(Device* device, Window* window);
		Swapchain() = delete;
		~Swapchain();

		inline const VkSwapchainKHR& hSwapchain() const { return m_swapchain; }
		inline VkExtent2D& hExtent2d() { return m_extent_2d; }
		inline const VkExtent2D& hExtent2d() const { return m_extent_2d; }
		inline const VkFormat& hFormat() const { return m_surface_format.format; }
		inline const std::vector<VkImageView>& hImageViews() const { return m_image_views; }

	private:
		VkDevice& m_device;
		VkPhysicalDevice& m_physical_device;
		GLFWwindow* m_window;
		VkSurfaceKHR& m_surface;

		uint32_t m_graphics_family;
		uint32_t m_present_family;

		VkSurfaceCapabilitiesKHR m_surface_capabilities;
		std::vector<VkSurfaceFormatKHR> m_surface_formats;
		std::vector<VkPresentModeKHR> m_present_modes;

		VkExtent2D m_extent_2d;
		VkSurfaceFormatKHR m_surface_format;
		VkPresentModeKHR m_present_mode;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_image_views;

		VkSwapchainKHR m_swapchain;

		void querySwapchainSupport();
		void setExtent2D();
		void setSurfaceFormat();
		void setSurfacePresentMode();
		void createSwapchain();
		void createImageViews();
	};
}
