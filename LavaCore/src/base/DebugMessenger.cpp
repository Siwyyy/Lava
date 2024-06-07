#include "DebugMessenger.hpp"

#include "Application.hpp"
#include "Initializers.hpp"

#include <iostream>

#include "Instance.hpp"

using namespace lvc;

DebugMessenger::DebugMessenger(const VkInstance& instance)
	: m_instance(instance)
	, m_debug_messenger(VK_NULL_HANDLE)
{
	if (!Instance::validation_layers_enabled)
		return;

	VkDebugUtilsMessengerCreateInfoEXT create_info = inits::debugCreateInfo();

	if (createDebugUtilsMessengerExt(&create_info, nullptr) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to setup Debug Utils Messenger!\n");
}

DebugMessenger::~DebugMessenger() noexcept(false)
{
	if (!Instance::validation_layers_enabled)
		return;

	destroyDebugUtilsMessengerExt(nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
	VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
	void* p_user_data)
{
	std::cerr << "[validation layer] " << p_callback_data->pMessage << "\n";

	return false;
}

VkResult DebugMessenger::createDebugUtilsMessengerExt(
	const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
	const VkAllocationCallbacks* p_allocator)
{
	const auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));

	if (create_func != nullptr)
		return create_func(m_instance, p_create_info, p_allocator, &m_debug_messenger);
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DebugMessenger::destroyDebugUtilsMessengerExt(
	const VkAllocationCallbacks* p_allocator) const
{
	const auto destroy_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));

	std::clog << "";
	if (destroy_func != nullptr)
	{
		destroy_func(m_instance, m_debug_messenger, p_allocator);
		std::clog << "Successfully destroyed Debug Utils Messenger\n";
	}
	else
		throw std::runtime_error("err: Failed to destroy Debug Utils Messenger!\n");
}
