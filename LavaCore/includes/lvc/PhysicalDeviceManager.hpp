#pragma once

#include <vector>

#include <vulkan/vulkan.h>

namespace lvc
{
	class PhysicalDevice;

	class PhysicalDeviceManager
	{
	public:
		explicit PhysicalDeviceManager(const VkInstance& t_instance);
		~PhysicalDeviceManager();

		inline const PhysicalDevice* hPhysicalDevice() const { return m_physical_device; }

	private:
		// Members
		std::vector<PhysicalDevice*> m_physical_device_vec;
		PhysicalDevice* m_physical_device;
	};
}
