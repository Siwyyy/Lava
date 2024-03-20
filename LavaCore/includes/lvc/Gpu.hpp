#pragma once

#include "QueueFamilyIndices.hpp"

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace lvc
{
	class Window;

	class Gpu
	{
	public:
		Gpu(VkPhysicalDevice t_physical_device,
				const VkSurfaceKHR& t_surface);
		Gpu()  = delete;
		~Gpu() = default;

		inline const VkPhysicalDevice& hVkPhysicalDevice() const { return m_physical_device; }
		inline const QueueFamilyIndices& hIndices() const { return m_indices; }
		inline const uint32_t& hScore() const { return m_score; }

		void logDeviceExtensions() const;
		void logRequiredExtensions() const;
		void logInfo() const;
		void logFullInfo() const;

		inline static const std::vector<const char*> required_extensions = {"VK_KHR_device_group",
																																				"VK_KHR_swapchain"};

	private:
		// Members
		VkPhysicalDevice m_physical_device;

		std::string m_name;
		std::vector<const char*> m_available_extensions;
		QueueFamilyIndices m_indices;

		bool m_extensions_good = false;
		uint32_t m_score       = 0;

		void queryExtensions();
		void checkExtensionSupport();
		void rateDeviceSuitability();
		void findQueueFamilies(const VkSurfaceKHR& t_surface);
	};
}
