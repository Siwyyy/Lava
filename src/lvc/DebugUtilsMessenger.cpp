#include "lvc/DebugUtilsMessenger.hpp"

#include <iostream>

using namespace lvc;

DebugUtilsMessenger::DebugUtilsMessenger(const Instance* instance) : m_debugMessenger(VK_NULL_HANDLE)
                                                                   , m_instance(instance)
{
	if (!Instance::validationLayersEnabled()) return;

	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	populateDebugUtilsMessengerInfo(create_info);

	if (createDebugUtilsMessengerExt(&create_info, nullptr) != VK_SUCCESS) throw std::runtime_error(
		"Failed to setup Debug Utils Messenger");
}

DebugUtilsMessenger::~DebugUtilsMessenger() noexcept(false)
{
	if (!Instance::validationLayersEnabled()) return;

	destroyDebugUtilsMessengerExt(nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessenger::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* p_user_data)
{
	std::cerr << "[validation layer]\t" << p_callback_data->pMessage << "\n";

	return false;
}

void DebugUtilsMessenger::populateDebugUtilsMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

VkResult DebugUtilsMessenger::createDebugUtilsMessengerExt(const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
                                                           const VkAllocationCallbacks* p_allocator)
{
	const auto create_func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		m_instance->handle(),
		"vkCreateDebugUtilsMessengerEXT"));

	if (create_func != nullptr) return create_func(m_instance->handle(), p_create_info, p_allocator, &m_debugMessenger);
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DebugUtilsMessenger::destroyDebugUtilsMessengerExt(const VkAllocationCallbacks* p_allocator) const
{
	const auto destroy_func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
		m_instance->handle(),
		"vkDestroyDebugUtilsMessengerEXT"));

	if (destroy_func != nullptr) destroy_func(m_instance->handle(), m_debugMessenger, p_allocator);
	else throw std::runtime_error("Failed to destroy Debug Utils Messenger");
}
