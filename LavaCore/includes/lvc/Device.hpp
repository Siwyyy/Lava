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

		inline PhysicalDevice* physicalDevice() const { return m_physical; }
		inline VkDevice hLogical() const { return m_logical; }

	private:
		Instance* m_instance;
		Window* m_window;

		std::vector<PhysicalDevice*> m_physical_devices;
		PhysicalDevice* m_physical;

		VkDevice m_logical;

		VkQueue m_graphics_queue;
		VkQueue m_present_queue;

		void pickPhysicalDevice();
	};
}
