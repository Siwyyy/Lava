#include "Instance.hpp"

#include "DebugMessenger.hpp"
#include "Log.hpp"

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>

#include <algorithm>

using namespace lava;

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
		LAVA_CORE_ERROR("Validation layers requested, but not available!");

	constexpr const char* app_name    = "LavaCore";
	constexpr uint32_t app_version    = VK_MAKE_API_VERSION(0, 0, 1, 0);
	constexpr const char* engine_name = "Wrrrum";
	constexpr uint32_t engine_version = VK_MAKE_API_VERSION(0, 0, 1, 0);

	VkApplicationInfo app_info;
	app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext              = nullptr;
	app_info.pApplicationName   = app_name;
	app_info.applicationVersion = app_version;
	app_info.pEngineName        = engine_name;
	app_info.engineVersion      = engine_version;
	app_info.apiVersion         = VK_API_VERSION_1_3;

	VkInstanceCreateInfo instance_info;
	instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pNext                   = nullptr;
	instance_info.flags                   = NULL;
	instance_info.pApplicationInfo        = &app_info;
	instance_info.enabledLayerCount       = 0;
	instance_info.ppEnabledLayerNames     = nullptr;
	instance_info.enabledExtensionCount   = static_cast<uint32_t>(m_required_extensions.size());
	instance_info.ppEnabledExtensionNames = m_required_extensions.data();

	if (validation_layers_enabled)
	{
		instance_info.enabledLayerCount   = static_cast<uint32_t>(validation_layers.size());
		instance_info.ppEnabledLayerNames = validation_layers.data();

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info = DebugMessenger::debugCreateInfo();
		instance_info.pNext                                  = &debug_create_info;
	}

	if (vkCreateInstance(&instance_info, nullptr, &m_instance) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create instance!");
}

Instance::~Instance()
{
	vkDestroyInstance(m_instance, nullptr);
}

void Instance::setupExtensions()
{
	m_required_extensions.append_range(default_extensions);

	queryExtensions();
	setRequiredExtensions();
	checkRequiredExtensions();
	logRequiredExtensions();
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

void Instance::logRequiredExtensions() const
{
	LAVA_CORE_INFO("Checking instance extensions...");
	bool found = false;
	for (const auto& required : m_required_extensions)
	{
		for (const auto& available : m_available_extensions)
		{
			if (!strcmp(required, available))
			{
				LAVA_CORE_DEBUG("(Available) {0}", required);
				found = true;
				break;
			}
		}
		if (!found)
			LAVA_CORE_ERROR("(Missing) {0}", required);
	}
	if(m_extensions_good)
		LAVA_CORE_INFO("All extensions good");
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
