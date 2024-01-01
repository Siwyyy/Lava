#include "lvc/DeviceExtensions.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace lvc;

DeviceExtensions::DeviceExtensions(const VkPhysicalDevice* physical)
	: m_physical(physical)
	, m_is_good(false)
{
	m_required_extension_names = {"VK_KHR_device_group",
																"VK_KHR_swapchain"};

	enumerateDeviceExtensions();
	m_is_good = checkRequiredExtensions();
}

void DeviceExtensions::enumerateDeviceExtensions()
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(*m_physical, nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> device_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(*m_physical, nullptr, &extension_count, device_extensions.data());
	m_available_extensions = device_extensions;

	std::clog << "Available device extensions:\n";
	for (const auto& extension : m_available_extensions)
		std::clog << extension.extensionName << '\n';
	std::clog << "--- --- --- --- ---\n\n";
}

bool DeviceExtensions::checkRequiredExtensions() const
{
	uint32_t found = 0;
	std::vector<std::pair<const char*, bool>> extensions;
	for (const auto& extension_name : m_required_extension_names)
	{
		extensions.emplace_back(extension_name, 0);
		for (const auto& available_ext : m_available_extensions)
		{
			if (!strcmp(extension_name, available_ext.extensionName))
			{
				extensions.back().second = true;
				found++;
			}
		}
	}

	std::clog << "\n--- --- --- --- ---\n";
	std::clog << "Available required device extensions:\n";
	for (const auto& extension : extensions)
		if (extension.second)
			std::clog << extension.first << '\n';
	std::clog << "--- --- --- --- ---\n";
	if (found == required().size())
	{
		std::clog << '\n';
		return 1;
	}
	else
	{
		std::clog << "Missing required device extensions:\n";
		for (const auto& extension : extensions)
			if (!extension.second)
				std::clog << extension.first << '\n';
		std::clog << "--- --- --- --- ---\n\n";
		return 0;
	}
}
