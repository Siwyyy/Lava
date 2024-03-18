#pragma once

#include "QueueFamilyIndices.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace lvc
{
	class Window;

	class PhysicalDevice
	{
	public:
		PhysicalDevice(const VkPhysicalDevice* t_physical_device, const Window& t_window);
		~PhysicalDevice() = default;

		inline const VkPhysicalDevice& hVkPhysicalDevice() const { return m_physical_device; }
		inline const QueueFamilyIndices& hIndices() const { return m_indices; }
		inline const uint32_t& hScore() const { return m_score; }

		void logDeviceExtensions() const;
		void logRequiredExtensions() const;
		void logInfo() const;

		inline static const std::vector<const char*> required_extensions = {"VK_KHR_device_group",
																																				"VK_KHR_swapchain"};

	private:
		const VkPhysicalDevice& m_physical_device;

		std::string m_name;
		std::vector<const char*> m_available_extensions;
		QueueFamilyIndices m_indices;

		bool m_extensions_good = false;
		uint32_t m_score       = 0;

		void queryExtensions();
		void checkExtensionSupport();
		void rateDeviceSuitability(const Window& window);
		void findQueueFamilies(VkSurfaceKHR surface);
	};
}
