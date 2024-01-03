#include "lvc/Instance.hpp"

#include "lvc/Application.hpp"
#include "lvc/DebugUtilsMessenger.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

using namespace lvc;

#ifdef NDEBUG
const bool Instance::enable_validation_layers = false;
#else
const bool Instance::enable_validation_layers = true;
#endif

const std::vector<const char*> Instance::validation_layers = {"VK_LAYER_KHRONOS_validation"};

Instance::InstanceExtensions::InstanceExtensions()
{
	m_required_extension_names = {"VK_KHR_surface",
																"VK_KHR_win32_surface",
																"VK_KHR_device_group_creation"};

	enumerateInstanceExtensions();
	getRequiredGlfwExtensions();
	checkRequiredExtensions();
}

void Instance::InstanceExtensions::enumerateInstanceExtensions()
{
	uint32_t extension_count;
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> extensions(extension_count);
	vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());
	m_available_extensions = extensions;

	std::clog << "--- --- --- --- ---\n";
	std::clog << "Available instance extensions:\n";
	for (const auto& extension : extensions)
		std::cout << extension.extensionName << '\n';
	std::clog << "--- --- --- --- ---\n";
}

void Instance::InstanceExtensions::getRequiredGlfwExtensions()
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

void Instance::InstanceExtensions::checkRequiredExtensions() const
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

Instance::Instance(const char* app_name, const char* engine_name)
	: m_instance(VK_NULL_HANDLE)

{
	if (enable_validation_layers && !checkValidationLayerSupport())
		throw std::runtime_error("err: Validation layers requested, but not available!\n");

	VkApplicationInfo app_info{};
	app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName   = app_name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName        = engine_name;
	app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion         = VK_API_VERSION_1_3;

	VkInstanceCreateInfo create_info{};
	create_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo        = &app_info;
	create_info.enabledExtensionCount   = static_cast<uint32_t>(m_instance_extensions.required().size());
	create_info.ppEnabledExtensionNames = m_instance_extensions.required().data();

	if (enable_validation_layers)
	{
		create_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
		DebugUtilsMessenger::populateDebugUtilsMessengerInfo(debug_create_info);
		create_info.pNext = &debug_create_info;
	}
	else
	{
		create_info.enabledLayerCount = 0;
		create_info.pNext             = nullptr;
	}

	if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create instance!\n");
}

Instance::~Instance()
{
	vkDestroyInstance(m_instance, nullptr);
	std::clog << "Successfully destroyed instance\n";
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
