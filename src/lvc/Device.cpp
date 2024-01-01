#include "lvc/Device.hpp"

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>

using namespace lvc;

Device::Device(const Instance* instance,
							 const Window* window)
	: m_physical(VK_NULL_HANDLE)
	, m_device_extensions(nullptr)
	, m_logical(VK_NULL_HANDLE)
	, m_graphics_queue(VK_NULL_HANDLE)
	, m_instance(instance)
	, m_window(window)
{
	pickPhysicalDevice();

	// Setup queue families
	std::set<uint32_t> unique_queue_families = {m_family_indices.graphics_family.value(),
																							m_family_indices.present_family.value()};

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	float priority = 1.0f;
	for (uint32_t queue_family : unique_queue_families)
	{
		VkDeviceQueueCreateInfo create_info = {};
		create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		create_info.queueFamilyIndex        = queue_family;
		create_info.queueCount              = 1;
		create_info.pQueuePriorities        = &priority;
		queue_create_infos.push_back(create_info);
	}

	// VkPhysicalDeviceFeatures device_futures = {};

	// Setup logical device
	VkDeviceCreateInfo create_info = {};
	create_info.sType              = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
	create_info.pQueueCreateInfos       = queue_create_infos.data();
	create_info.pEnabledFeatures        = nullptr;
	create_info.enabledExtensionCount   = static_cast<uint32_t>(m_device_extensions->required().size());
	create_info.ppEnabledExtensionNames = m_device_extensions->required().data();

	if (Instance::validationLayersEnabled())
	{
		create_info.enabledLayerCount   = static_cast<uint32_t>(Instance::validation_layers.size());
		create_info.ppEnabledLayerNames = Instance::validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	if (vkCreateDevice(m_physical, &create_info, nullptr, &m_logical) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create logical device!\n");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_logical, m_family_indices.graphics_family.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_logical, m_family_indices.present_family.value(), 0, &m_present_queue);
}

Device::~Device()
{
	vkDestroyDevice(m_logical, nullptr);
	std::clog << "Successfully destroyed device\n";
}

void Device::pickPhysicalDevice()
{
	uint32_t device_count = 0;
	vkEnumeratePhysicalDevices(m_instance->handle(), &device_count, nullptr);

	if (device_count == 0)
		throw std::runtime_error("err: Failed to find GPUs with Vulkan support!\n");

	std::vector<VkPhysicalDevice> devices(device_count);
	vkEnumeratePhysicalDevices(m_instance->handle(), &device_count, devices.data());

	std::multimap<int, std::pair<VkPhysicalDevice, DeviceExtensions>> candidates;
	for (const auto& device : devices)
	{
		const QueueFamilyIndices* queue_families = findQueueFamilies(device, m_window->hSurface());
		DeviceExtensions device_extensions(&device);
		int score;
		if (!queue_families->isComplete() || !device_extensions.isGood())
			score = 0;
		else
			score = rateDeviceSuitability(device, *m_window);
		candidates.insert(std::make_pair(score, std::make_pair(device, device_extensions)));
	}

	if (candidates.rbegin()->first == 0)
		throw std::runtime_error("err: Failed to find suitable GPU!\n");

	m_physical = candidates.rbegin()->second.first;
	m_device_extensions = &candidates.rbegin()->second.second;
	m_family_indices    = findQueueFamilies(m_physical, m_window->hSurface());
}

int Device::rateDeviceSuitability(const VkPhysicalDevice& physical, const Window& window)
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(physical, &device_properties);
	vkGetPhysicalDeviceFeatures(physical, &device_features);

	std::clog << "\n--- --- --- --- ---\n";
	std::cout << "GPU: " << device_properties.deviceName << '\n';

	int score = 0;

	if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;
	score += static_cast<int>(device_properties.limits.maxImageDimension2D);

	if (!device_features.geometryShader)
		return 0;

	return score;
}

QueueFamilyIndices* Device::findQueueFamilies(const VkPhysicalDevice& physical, const VkSurfaceKHR surface)
{
	QueueFamilyIndices* indices;

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
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			indices.graphics_family = i;

		// Check for surface presentation support
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical, i, surface, &present_support);
		if (present_support)
			indices.present_family = i;

		found = indices.isComplete();
	}

	return indices;
}
