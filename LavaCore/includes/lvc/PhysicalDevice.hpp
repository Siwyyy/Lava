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
		PhysicalDevice(const VkPhysicalDevice* physical, const Window& window);
		~PhysicalDevice() = default;

		inline VkPhysicalDevice& physical() { return m_physical; }
		inline const QueueFamilyIndices* indices() const { return &m_indices; }
		inline const uint32_t& score() const { return m_score; }

		void logDeviceExtensions() const;
		void logRequiredExtensions() const;
		void logInfo() const;

		inline static const std::vector<const char*> required_extensions = {"VK_KHR_device_group",
																																						 "VK_KHR_swapchain"};

	private:
		VkPhysicalDevice m_physical;

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
