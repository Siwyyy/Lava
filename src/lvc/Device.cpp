#include "lvc/Device.hpp"

#include "lvc/QueueFamily.hpp"

#include <iostream>
#include <map>

using namespace lvc;

Device::Device(const Instance& instance,
					const Window& window)
	:m_physical(VK_NULL_HANDLE)
	,m_instance(instance)
	,m_window(window)
{
	pickPhysicalDevice();

}

Device::~Device()
{
}

void Device::pickPhysicalDevice()
{
	// Check for GPUs with Vulkan support
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance.handle(), &deviceCount, &m_physical);

	if (deviceCount == 0)
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	// Get available GPUs
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance.handle(), &deviceCount, devices.data());

	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices)
	{
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	if (candidates.rbegin()->first > 0)
		m_physical = candidates.rbegin()->second;
	else
		throw std::runtime_error("Failed to find suitable GPU!");
}

int Device::rateDeviceSuitability(VkPhysicalDevice device)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	score += deviceProperties.limits.maxImageDimension2D;

	if (!deviceFeatures.geometryShader)
		return 0;

	return score;
}

bool Device::isDeviceSuitable(const VkPhysicalDevice& device)
{
	QueueFamilyIndices indices = QueueFamily::FindQueueFamilies(device);

	return indices.isComplete();
}
