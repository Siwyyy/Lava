#include "GpuManager.hpp"

#include "Gpu.hpp"

#include <iostream>
#include <stdexcept>

using namespace lava;

GpuManager::GpuManager(const VkInstance& t_instance, const VkSurfaceKHR& t_surface)
	: m_gpu()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(t_instance, &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("err: Failed to find GPUs with Vulkan support!\n");

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(t_instance, &device_count, devices.data());

	for (const auto& device : devices)
		m_available_gpu.emplace_back(device, t_surface);

	logAvailableGpu();
	selectGpu();
	logSelectedGpu();
}

void GpuManager::selectGpu()
{
	if(m_available_gpu.empty())
		throw std::runtime_error("err: List of available GPU is empty!\n");

	m_gpu = m_available_gpu[0];
	for (const Gpu& gpu : m_available_gpu)
	{
		if (gpu.hScore() > m_gpu.hScore())
			m_gpu = gpu;
	}

	if (!m_gpu.hScore())
		throw std::runtime_error("err: Failed to find suitable GPU!\n");
}

void GpuManager::logAvailableGpu() const
{
	std::cout << "=== === GPU found: === ===\n";
	for (const auto& gpu : m_available_gpu)
		gpu.logInfo();
	std::cout << "=== === === == === === ===\n";
}

void GpuManager::logSelectedGpu() const
{
	std::cout << "=== == Selected GPU: == ==\n";
	m_gpu.logFullInfo();
	std::cout << "=== === === == === === ===\n";
}
