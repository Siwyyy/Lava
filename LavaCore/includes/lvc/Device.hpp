#pragma once

#include "lvc/Instance.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class PhysicalDevice;
	class Window;

	struct QueueFamilyIndices;

	class Device
	{
	public:
		Device(const VkPhysicalDevice& t_physical_device,
					 const QueueFamilyIndices& t_queue_family_indices
		);
		~Device();

		inline VkDevice& hVkDevice() { return m_device; }
		inline const VkDevice& hVkDevice() const { return m_device; }
		inline const VkQueue& hGraphicsQueue() const { return m_graphics_queue; }
		inline const VkQueue& hPresentQueue() const { return m_present_queue; }

	private:
		// Out Refs
		const VkPhysicalDevice& m_physical_device;

		// Members
		VkDevice m_device;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;
	};
}
