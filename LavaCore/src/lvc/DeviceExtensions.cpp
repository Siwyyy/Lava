#include "lvc/DeviceExtensions.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace lvc;

const std::vector<const char*> DeviceExtensions::required_extension_names = {
	"VK_KHR_device_group",
	"VK_KHR_swapchain"};

DeviceExtensions::DeviceExtensions(const VkPhysicalDevice* physical)
	: m_physical(physical)
	, m_is_good(false)
{
	enumerateDeviceExtensions();

	uint32_t found = 0;
	for (const auto& extension_name : required_extension_names)
	{
		for (const auto& available_ext : m_available_extensions)
		{
			if (!strcmp(extension_name, available_ext.extensionName))
			{
				found++;
			}
		}
	}
	m_is_good = found == required_extension_names.size();
}

void DeviceExtensions::enumerateDeviceExtensions()
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(*m_physical, nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> device_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(*m_physical, nullptr, &extension_count, device_extensions.data());
	m_available_extensions = device_extensions;
}

void DeviceExtensions::logDeviceExtensions() const
{
	std::clog << "Available device extensions:\n";
	for (const auto& extension : m_available_extensions)
		std::clog << extension.extensionName << '\n';
}

void DeviceExtensions::logRequiredExtensions() const
{
	std::clog << "Required device extensions:\n";
	for (const auto& extension_name : required_extension_names)
	{
		bool available = false;
		std::clog << extension_name;
		for (const auto& available_extension : m_available_extensions)
		{
			if (!strcmp(extension_name, available_extension.extensionName))
			{
				std::clog << " (Available)\n";
				available = true;
				break;
			}
		}
		if (!available)
			std::clog << " (Not available)\n";
	}
}
