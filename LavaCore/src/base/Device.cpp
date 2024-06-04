#include "Device.hpp"

#include "Gpu.hpp"
#include "Instance.hpp"
#include "Window.hpp"

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>
#include <vulkan/vulkan.h>

using namespace lvc;

Device::Device(const VkPhysicalDevice& t_physical_device,
							 const QueueFamilyIndices& t_queue_family_indices)
	: m_physical_device(t_physical_device)
	, m_device(VK_NULL_HANDLE)
	, m_graphics_queue(VK_NULL_HANDLE)
	, m_present_queue(VK_NULL_HANDLE)
{
	// Setup queue families
	const std::set<uint32_t> unique_queue_families = {t_queue_family_indices.graphics.value(),
																										t_queue_family_indices.present.value()};

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	constexpr float priority = 1.0f;
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
	create_info.enabledExtensionCount   = static_cast<uint32_t>(Gpu::required_extensions.size());
	create_info.ppEnabledExtensionNames = Gpu::required_extensions.data();

	if (Instance::validationLayersEnabled())
	{
		create_info.enabledLayerCount   = static_cast<uint32_t>(Instance::validation_layers.size());
		create_info.ppEnabledLayerNames = Instance::validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create logical device!\n");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_device, t_queue_family_indices.graphics.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_device, t_queue_family_indices.present.value(), 0, &m_present_queue);
}

Device::~Device()
{
	vkDestroyDevice(m_device, nullptr);
	std::clog << "Successfully destroyed device\n";
}
