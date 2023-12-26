#include "lvc/Device.hpp"

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"
#include "lvc/QueueFamilyIndices.hpp"

#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace lvc;

Device::Device(const Instance* instance, const Window* window,
               const std::vector<const char*>& extensions) : m_physical(VK_NULL_HANDLE)
                                                           , m_logical(VK_NULL_HANDLE)
                                                           , m_graphics_queue(VK_NULL_HANDLE)
                                                           , m_instance(instance)
                                                           , m_window(window)
{
	pickPhysicalDevice();
	m_family_indices = findQueueFamilies(m_physical);

	// Setup queue families
	std::set<uint32_t> unique_queue_families = {m_family_indices.graphics_family.value()};
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

	float priority = 1.0f;
	for (uint32_t queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex = queue_family;
		create_info.queueCount = 1;
		create_info.pQueuePriorities = &priority;
		queue_create_infos.push_back(create_info);
	}

	VkPhysicalDeviceFeatures device_feautures = {};

	// Setup logical device
	VkDeviceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.pEnabledFeatures = nullptr;

	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	if (lvc::Instance::validationLayersEnabled())
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(lvc::Instance::validation_layers.size());
		create_info.ppEnabledLayerNames = lvc::Instance::validation_layers.data();
	}
	else create_info.enabledLayerCount = 0;

	if (vkCreateDevice(m_physical, &create_info, nullptr, &m_logical) != VK_SUCCESS)
		throw std::runtime_error("failed to create logical device!");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_logical, m_family_indices.graphics_family.value(), 0, &m_graphics_queue);
}

Device::~Device() { vkDestroyDevice(m_logical, nullptr); }

void Device::pickPhysicalDevice()
{
	// Check for GPUs with Vulkan support
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(m_instance->handle(), &device_count, nullptr);

	if (device_count == 0) throw std::runtime_error("Failed to find GPUs with Vulkan support!");

	// Get available GPUs
	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance->handle(), &device_count, devices.data());

	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto& device : devices)
	{
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	if (candidates.rbegin()->first > 0) m_physical = candidates.rbegin()->second;
	else throw std::runtime_error("Failed to find suitable GPU!");
}

int Device::rateDeviceSuitability(const VkPhysicalDevice physical)
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(physical, &device_properties);
	vkGetPhysicalDeviceFeatures(physical, &device_features);

	std::cout << "GPU found: " << device_properties.deviceName << '\n';

	int score = 0;

	if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

	score += static_cast<int>(device_properties.limits.maxImageDimension2D);

	if (!device_features.geometryShader) return 0;

	return score;
}

bool Device::isDeviceSuitable(const VkPhysicalDevice& physical)
{
	const QueueFamilyIndices indices = findQueueFamilies(physical);

	return indices.isComplete();
}

QueueFamilyIndices Device::findQueueFamilies(const VkPhysicalDevice& physical)
{
	QueueFamilyIndices indices;

	// Get queue families
	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(physical, &family_count, families.data());

	// iterate through families until one that supports requirements is found
	bool found = false;
	for (int i = 0; !found && i < families.size(); i++)
	{
		const auto& family = families[i];

		//  Check for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) { indices.graphics_family = i; }

		found = indices.isComplete();
	}

	return indices;
}
