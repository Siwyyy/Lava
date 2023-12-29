#include "lvc/InstanceExtensions.hpp"

#include <iostream>
#include <utility>
#include <GLFW/glfw3.h>

#include "lvc/Instance.hpp"

using namespace lvc;

InstanceExtensions::InstanceExtensions()
{
	m_required_extension_names = {"VK_KHR_surface",
																"VK_KHR_win32_surface",
																"VK_KHR_device_group_creation"};

	enumerateInstanceExtensions();
	getRequiredGlfwExtensions();
	checkRequiredExtensions();
}

void InstanceExtensions::enumerateInstanceExtensions()
{
	uint32_t extension_count;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	m_available_extensions = extensions;

	std::clog << "\n--- --- --- --- ---\n";
	std::clog << "Available instance extensions:\n";
	for (const auto& extension : extensions)
		std::cout << extension.extensionName << '\n';
	std::clog << "--- --- --- --- ---\n\n";
}

void InstanceExtensions::getRequiredGlfwExtensions()
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions  = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> extensions = {glfw_extensions,glfw_extensions + glfw_extension_count};

	if (Instance::validationLayersEnabled())
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	m_required_extension_names.append_range(extensions);
}

void InstanceExtensions::checkRequiredExtensions() const
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
	std::clog << "Available required instance extensions:\n";
	for (const auto& extension : extensions)
		if (extension.second)
			std::clog << extension.first << '\n';
	std::clog << "--- --- --- --- ---\n";
	if (found == required().size())
		std::clog << '\n';
	else
	{
		std::clog << "Missing required instance extensions:\n";
		for (const auto& extension : extensions)
			if (!extension.second)
				std::clog << extension.first << '\n';
		std::clog << "--- --- --- --- ---\n\n";
	}
}
