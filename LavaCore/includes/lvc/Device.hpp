#pragma once

#include "QueueFamilyIndices.hpp"
#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class DeviceExtensions;
	class PhysicalDevice;

	class Device
	{
	public:
		Device(Instance* instance, Window* window);
		~Device();

		inline PhysicalDevice* physicalDevice() const { return m_physical_device; }
		inline VkDevice& hDevice() { return m_device; }
		inline const VkDevice& hDevice() const { return m_device; }

	private:
		Instance* m_instance;
		Window* m_window;

		std::vector<PhysicalDevice*> m_physical_device_vec;
		PhysicalDevice* m_physical_device;

		VkDevice m_device;

		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		void pickPhysicalDevice();
	};
}
