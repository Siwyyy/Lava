#include "Instance.hpp"

#include "DebugMessenger.hpp"

#define GLFW_INCLUDE_VULKAN
#include "Initializers.hpp"

#include <glfw3.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace lvc;

#ifdef NDEBUG
const bool Instance::validation_layers_enabled = false;
#else
const bool Instance::validation_layers_enabled = true;
#endif

Instance::Instance()
	: m_instance(VK_NULL_HANDLE)
{
	setupExtensions();

	// ReSharper disable once CppRedundantBooleanExpressionArgument
	if (validation_layers_enabled && !checkValidationLayerSupport())
		throw std::runtime_error("err: Validation layers requested, but not available!\n");

	VkInstanceCreateInfo instance_create_info    = inits::instanceInfo();
	instance_create_info.enabledExtensionCount   = static_cast<uint32_t>(m_required_extensions.size());
	instance_create_info.ppEnabledExtensionNames = m_required_extensions.data();

	if (validation_layers_enabled)
	{
		instance_create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
		instance_create_info.ppEnabledLayerNames = validation_layers.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info = inits::debugCreateInfo();
		instance_create_info.pNext                           = &debug_create_info;
	}

	if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create instance!\n");
}

Instance::~Instance()
{
	vkDestroyInstance(m_instance, nullptr);
	std::clog << "Successfully destroyed instance\n";
}

void Instance::setupExtensions()
{
	m_required_extensions.append_range(default_extensions);

	queryExtensions();
	setRequiredExtensions();
	checkRequiredExtensions();

	std::clog << "=== === === == === === ===\n";
	// logAvailableExtensions();
	// std::clog << "=== === == === === == === ===\n";
	logRequiredExtensions();
	std::clog << "=== === === == === === ===\n";
}

void Instance::queryExtensions()
{
	uint32_t extension_count;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

	for (const auto& extension : extensions)
	{
		const std::string* name = new std::string(extension.extensionName);
		m_available_extensions.emplace_back(name->c_str());
	}
}

void Instance::setRequiredExtensions()
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions  = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

	std::vector<const char*> extensions = {glfw_extensions,glfw_extensions + glfw_extension_count};

	if (validation_layers_enabled)
	{
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	m_required_extensions.append_range(extensions);
}

void Instance::checkRequiredExtensions()
{
	uint32_t found = 0;
	for (const auto& required : m_required_extensions)
		for (const auto& available : m_available_extensions)
			if (!strcmp(required, available))
			{
				found++;
				break;
			}

	m_extensions_good = found == m_required_extensions.size();
}

void Instance::logAvailableExtensions() const
{
	std::clog << "Available instance extensions:\n";
	for (const auto& extension : m_available_extensions)
		std::cout << extension << '\n';
}

void Instance::logRequiredExtensions() const
{
	std::clog << "Required instance extensions:\n";
	bool found = false;
	for (const auto& required : m_required_extensions)
	{
		for (const auto& available : m_available_extensions)
		{
			if (!strcmp(required, available))
			{
				std::clog << "\t(Available)\t";
				found = true;
				break;
			}
		}
		if (!found)
			std::clog << "\t(Not available)\t";
		std::clog << required << '\n';
	}
}

bool Instance::checkValidationLayerSupport()
{
	uint32_t layer_count;
	vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

	std::vector<VkLayerProperties> available_layers(layer_count);
	vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

	for (const char* layer_name : validation_layers)
	{
		bool layer_found = false;

		for (const auto& layer_properties : available_layers)
		{
			if (strcmp(layer_name, layer_properties.layerName) == 0)
			{
				layer_found = true;
				break;
			}
		}

		if (!layer_found)
		{
			return false;
		}
	}

	return true;
}