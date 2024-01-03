#include "lvc/Device.hpp"

#include "lvc/Instance.hpp"
#include "lvc/Window.hpp"

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

using namespace lvc;

Device::Device(const Instance* instance,
							 const Window* window)
	: m_physical(VK_NULL_HANDLE)
	, m_device_extensions(nullptr)
	, m_family_indices(nullptr)
	, m_logical(VK_NULL_HANDLE)
	, m_graphics_queue(VK_NULL_HANDLE)
	, m_instance(instance)
	, m_window(window)
{
	pickPhysicalDevice();

	// Setup queue families
	const std::set<uint32_t> unique_queue_families = {m_family_indices->graphics_family.value(),
																										m_family_indices->present_family.value()};

	std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
	float priority = 1.0f;
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
	create_info.enabledExtensionCount   = static_cast<uint32_t>(DeviceExtensions::required_extension_names.size());
	create_info.ppEnabledExtensionNames = DeviceExtensions::required_extension_names.data();

	if (Instance::validationLayersEnabled())
	{
		create_info.enabledLayerCount   = static_cast<uint32_t>(Instance::validation_layers.size());
		create_info.ppEnabledLayerNames = Instance::validation_layers.data();
	}
	else
		create_info.enabledLayerCount = 0;

	if (vkCreateDevice(*m_physical, &create_info, nullptr, &m_logical) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create logical device!\n");

	// Get handles for graphics and presentation queues
	vkGetDeviceQueue(m_logical, m_family_indices->graphics_family.value(), 0, &m_graphics_queue);
	vkGetDeviceQueue(m_logical, m_family_indices->present_family.value(), 0, &m_present_queue);
}

Device::~Device()
{
	vkDestroyDevice(m_logical, nullptr);
	std::clog << "Successfully destroyed device\n";
}

Device::PhysicalDevice::PhysicalDevice(const VkPhysicalDevice* physical, const Window& window)
	: m_physical(new VkPhysicalDevice(*physical))
	, m_extensions(nullptr)
	, m_indices(nullptr)
	, m_score(0)
{
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(*m_physical, &device_properties);
	m_name.append_range(device_properties.deviceName);
	m_extensions = new DeviceExtensions(m_physical);
	findQueueFamilies(window.hSurface());

	rateDeviceSuitability(window);
}

Device::PhysicalDevice::~PhysicalDevice()
{
	delete m_extensions;
	delete m_indices;
}

void Device::PhysicalDevice::logInfo() const
{
	std::clog << "\n--- --- --- --- ---\n";
	std::cout << "GPU: " << m_name << '\n';
	std::clog << "--- --- --- --- ---\n";
	m_extensions->logDeviceExtensions();
	std::clog << "--- --- --- --- ---\n";
	m_extensions->logRequiredExtensions();
	std::clog << "--- --- --- --- ---\n\n";
}

void Device::PhysicalDevice::rateDeviceSuitability(const Window& window)
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(*m_physical, &device_properties);
	vkGetPhysicalDeviceFeatures(*m_physical, &device_features);

	if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		m_score += 1000;
	m_score += static_cast<int>(device_properties.limits.maxImageDimension2D);

	if (!device_features.geometryShader
			|| !m_indices->isComplete()
			|| !m_extensions->isGood())
		m_score = 0;
}

void Device::PhysicalDevice::findQueueFamilies(const VkSurfaceKHR surface)
{
	m_indices = new QueueFamilyIndices;

	// Get queue families
	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(*m_physical, &family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(*m_physical, &family_count, families.data());

	// iterate through families until one that supports requirements is found
	bool found = false;
	for (int i = 0; !found && i < families.size(); i++)
	{
		const auto& family = families[i];

		//  Check for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			m_indices->graphics_family = i;

		// Check for surface presentation support
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(*m_physical, i, surface, &present_support);
		if (present_support)
			m_indices->present_family = i;

		found = m_indices->isComplete();
	}
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
		m_physical_devices.emplace_back(new PhysicalDevice(&device, *m_window));

	const PhysicalDevice* pick = nullptr;
	for (const PhysicalDevice* device : m_physical_devices)
	{
		device->logInfo();
		if (!pick)
		{
			if (device->score())
				pick = device;
			continue;
		}
		if (device->score() > pick->score())
			pick = device;
	}

	if (!pick)
		throw std::runtime_error("err: Failed to find suitable GPU!\n");

	m_physical          = pick->physical();
	m_device_extensions = pick->extensions();
	m_family_indices    = pick->indices();
}
