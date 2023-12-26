#include "lvc/Instance.hpp"

#include "lvc/DebugUtilsMessenger.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>

using namespace lvc;

#ifdef NDEBUG
const bool Instance::m_enableValidationLayers = false;
#else
const bool Instance::enable_validation_layers = true;
#endif

const std::vector<const char*> Instance::validation_layers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> Instance::device_extensions = {};

Instance::Instance(const char* app_name, const char* engine_name) : m_instance(VK_NULL_HANDLE)

{
	if (enable_validation_layers && !checkValidationLayerSupport()) throw std::runtime_error(
		"validation layers requested, but not available!");

	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = app_name;
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = engine_name;
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo create_info{};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	std::vector<const char*> extensions;
	getRequiredExtensions(extensions);
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	create_info.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
	if (enable_validation_layers)
	{
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();

		DebugUtilsMessenger::populateDebugUtilsMessengerInfo(debug_create_info);
		create_info.pNext = &debug_create_info;
	}
	else
	{
		create_info.enabledLayerCount = 0;
		create_info.pNext = nullptr;
	}

	if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS) throw std::runtime_error(
		"failed to create instance!");
}

Instance::~Instance() { vkDestroyInstance(m_instance, nullptr); }

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

		if (!layer_found) { return false; }
	}

	return true;
}

void Instance::getRequiredExtensions(std::vector<const char*>& extensions)
{
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	extensions = {glfw_extensions, glfw_extensions + glfw_extension_count};

	if (enable_validation_layers) { extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); }
}
