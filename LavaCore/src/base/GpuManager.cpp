#include "GpuManager.hpp"

#include "Gpu.hpp"
#include "Log.hpp"

using namespace lava;

GpuManager::GpuManager(const VkInstance& t_instance, const VkSurfaceKHR& t_surface)
	: m_gpu()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(t_instance, &device_count, nullptr);

	if (device_count == 0)
		LAVA_CORE_ERROR("Failed to find GPUs with Vulkan support!");

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
	if (m_available_gpu.empty())
		LAVA_CORE_ERROR("List of available GPU is empty!");

	m_gpu = m_available_gpu[0];
	for (const Gpu& gpu : m_available_gpu)
	{
		if (gpu.hScore() > m_gpu.hScore())
			m_gpu = gpu;
	}

	if (!m_gpu.hScore())
		LAVA_CORE_ERROR("Failed to find suitable GPU!");
}

void GpuManager::logAvailableGpu() const
{
	for (const auto& gpu : m_available_gpu)
		gpu.logFullInfo();
}

void GpuManager::logSelectedGpu() const
{
	m_gpu.logInfo();
}
