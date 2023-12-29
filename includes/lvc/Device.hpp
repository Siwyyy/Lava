#pragma once

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include "DeviceExtensions.hpp"

namespace lvc
{
	struct QueueFamilyIndices
	{
	public:
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		inline bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
	};

	class Device
	{
	public:
		Device(const Instance* instance, const Window* window);
		~Device();

		inline VkPhysicalDevice hPhysical() const { return m_physical; }
		inline VkDevice hLogical() const { return m_logical; }
		inline const QueueFamilyIndices& hFamilyIndices() const { return m_family_indices; }

	private:
		VkPhysicalDevice m_physical;
		DeviceExtensions* m_device_extensions;
		VkDevice m_logical;

		QueueFamilyIndices m_family_indices;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		const Instance* m_instance;
		const Window* m_window;

		void pickPhysicalDevice();
		static int rateDeviceSuitability(VkPhysicalDevice physical);
		static bool isDeviceSuitable(const VkPhysicalDevice& physical, const Window& window);
		static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physical, const VkSurfaceKHR surface);
	};
}
