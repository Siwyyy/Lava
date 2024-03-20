#include "lvc/GpuManager.hpp"

#include "lvc/Gpu.hpp"

#include <iostream>
#include <stdexcept>

using namespace lvc;

GpuManager::GpuManager(const VkInstance& t_instance, const VkSurfaceKHR& t_surface)
	: m_gpu(nullptr)
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(t_instance, &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("err: Failed to find GPUs with Vulkan support!\n");

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(t_instance, &device_count, devices.data());

	for (const auto& device : devices)
		m_available_gpu.emplace_back(new Gpu(device, t_surface));

	logAvailableGpu();
	selectGpu();
	logSelectedGpu();
}

void GpuManager::selectGpu()
{
	for (Gpu* device : m_available_gpu)
	{
		if (!m_gpu)
		{
			if (device->hScore())
				m_gpu = device;
			continue;
		}
		if (device->hScore() > m_gpu->hScore())
			m_gpu = device;
	}

	if (!m_gpu)
		throw std::runtime_error("err: Failed to find suitable GPU!\n");
}

void GpuManager::logAvailableGpu() const
{
	std::cout << "=== === GPU found: === ===\n";
	for (const auto& device : m_available_gpu)
		device->logInfo();
	std::cout << "=== === === == === === ===\n";
}

void GpuManager::logSelectedGpu() const
{
	std::cout << "=== === Selected GPU: === ===\n";
	m_gpu->logFullInfo();
	std::cout << "=== === == === === == === ===\n";
}
