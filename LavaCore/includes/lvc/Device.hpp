#pragma once

#include "lvc/Instance.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class PhysicalDevice;
	class Window;

	class Device
	{
	public:
		Device(Instance* instance, Window* window);
		~Device();

		inline PhysicalDevice* physicalDevice() const { return m_physical_device; }
		inline VkDevice& hDevice() { return m_device; }
		inline const VkDevice& hDevice() const { return m_device; }
		inline const VkQueue& hGraphicsQueue() const { return m_graphics_queue; }
		inline const VkQueue& hPresentQueue() const { return m_present_queue; }

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
