#include "lvc/PhysicalDevice.hpp"

#include "lvc/Window.hpp"

#include <iostream>

using namespace lvc;

PhysicalDevice::PhysicalDevice(const VkPhysicalDevice* t_physical_device, const Window& t_window)
	: m_physical_device(*t_physical_device)
{
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
	m_name.append_range(device_properties.deviceName);
	queryExtensions();
	checkExtensionSupport();
	findQueueFamilies(t_window.hVkSurface());

	rateDeviceSuitability(t_window);
}

void PhysicalDevice::logDeviceExtensions() const
{
	std::clog << "Available device extensions:\n";
	for (const auto& extension : m_available_extensions)
		std::clog << extension << '\n';
}

void PhysicalDevice::logRequiredExtensions() const
{
	std::clog << "Required device extensions:\n";
	for (const auto& required : required_extensions)
	{
		bool found = false;
		std::clog << required;
		for (const auto& available : m_available_extensions)
		{
			if (!strcmp(required, available))
			{
				std::clog << " (Available)\n";
				found = true;
				break;
			}
		}
		if (!found)
			std::clog << " (Not available)\n";
	}
}

void PhysicalDevice::logInfo() const
{
	std::clog << "\n--- --- --- --- --- --- --- --- --- ---\n";
	std::cout << "GPU: " << m_name << '\n';
	std::clog << "--- --- --- --- --- --- --- --- --- ---\n";
	// logDeviceExtensions();
	// std::clog << "--- --- --- --- --- --- --- --- --- ---\n";
	logRequiredExtensions();
	std::clog << "--- --- --- --- --- --- --- --- --- ---\n\n";
}

void PhysicalDevice::queryExtensions()
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> device_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, device_extensions.data());

	for (const auto& extension : device_extensions)
	{
		const std::string* name = new std::string(extension.extensionName);
		m_available_extensions.emplace_back(name->c_str());
	}
}

void PhysicalDevice::checkExtensionSupport()
{
	uint32_t found = 0;
	for (const auto& required : required_extensions)
		for (const auto& available : m_available_extensions)
			if (!strcmp(required, available))
			{
				found++;
			}

	m_extensions_good = found == required_extensions.size();
}

void PhysicalDevice::rateDeviceSuitability(const Window& window)
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
	vkGetPhysicalDeviceFeatures(m_physical_device, &device_features);

	if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		m_score += 1000;
	m_score += static_cast<int>(device_properties.limits.maxImageDimension2D);

	if (!device_features.geometryShader
			|| !m_indices.isComplete()
			|| !m_extensions_good)
		m_score = 0;
}

void PhysicalDevice::findQueueFamilies(const VkSurfaceKHR surface)
{
	// Get queue families
	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, families.data());

	// iterate through families until one that supports requirements is found
	bool found = false;
	for (int i = 0; !found && i < families.size(); i++)
	{
		const auto& family = families[i];

		//  Check for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			m_indices.graphics_family = i;

		// Check for surface presentation support
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, surface, &present_support);
		if (present_support)
			m_indices.present_family = i;

		found = m_indices.isComplete();
	}
}
