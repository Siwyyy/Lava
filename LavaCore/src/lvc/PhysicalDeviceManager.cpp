#include "lvc/PhysicalDeviceManager.hpp"

#include "lvc/PhysicalDevice.hpp"

#include <stdexcept>

using namespace lvc;

PhysicalDeviceManager::PhysicalDeviceManager(const VkInstance& t_instance)
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(t_instance, &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("err: Failed to find GPUs with Vulkan support!\n");

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(t_instance, &device_count, devices.data());

	for (const auto& device : devices)
		m_physical_device_vec.emplace_back(new PhysicalDevice(&device, *m_window)); //ToDo will be deleted from this function

	for (PhysicalDevice* device : m_physical_device_vec)
	{
		device->logInfo();
		if (!m_physical_device)
		{
			if (device->hScore())
				m_physical_device = device;
			continue;
		}
		if (device->hScore() > m_physical_device->hScore())
			m_physical_device = device;
	}

	if (!m_physical_device)
		throw std::runtime_error("err: Failed to find suitable GPU!\n");
}

lvc::PhysicalDeviceManager::~PhysicalDeviceManager() {}
