#include "lvc/Device.hpp"

#include "lvc/Instance.hpp"
#include "lvc/PhysicalDevice.hpp"
#include "lvc/Window.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <vulkan/vulkan.h>

using namespace lvc;

Device::Device(Instance* instance,
							 Window* window)
	: m_instance(instance)
	, m_window(window)
	, m_physical_device(VK_NULL_HANDLE)
	, m_device(VK_NULL_HANDLE)
	, m_graphics_queue(VK_NULL_HANDLE)
{
	pickPhysicalDevice();

	// Setup queue families
	const std::set<uint32_t> unique_queue_families = {m_physical_device->indices()->graphics_family.value(),
																										m_physical_device->indices()->present_family.value()};

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	const float priority = 1.0f;
	for (const uint32_t& queue_family : unique_queue_families)
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
	create_info.enabledExtensionCount   = static_cast<uint32_t>(PhysicalDevice::required_extensions.size());
	create_info.ppEnabledExtensionNames = PhysicalDevice::required_extensions.data();

	if (Instance::validationLayersEnabled())
	{
		create_info.enabledLayerCount   = static_cast<uint32_t>(Instance::validation_layers.size());
		create_info.ppEnabledLayerNames = Instance::validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	if (vkCreateDevice(m_physical_device->physical(), &create_info, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create logical device!\n");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_device, m_physical_device->indices()->graphics_family.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, m_physical_device->indices()->present_family.value(), 0, &m_present_queue);
}

Device::~Device()
{
	vkDestroyDevice(m_device, nullptr);
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

	for (const auto& device : devices)
		m_physical_device_vec.emplace_back(new PhysicalDevice(&device, *m_window));

	for (PhysicalDevice* device : m_physical_device_vec)
	{
		device->logInfo();
		if (!m_physical_device)
		{
			if (device->score())
				m_physical_device = device;
			continue;
		}
		if (device->score() > m_physical_device->score())
			m_physical_device = device;
	}

	if (!m_physical_device)
		throw std::runtime_error("err: Failed to find suitable GPU!\n");
}
