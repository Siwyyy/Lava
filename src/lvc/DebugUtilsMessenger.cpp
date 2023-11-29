#include "lvc/DebugUtilsMessenger.hpp"

#include <iostream>

using namespace lvc;

lvc::DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance)
	: m_debugMessanger(VK_NULL_HANDLE)
	, m_instance(instance)
{
	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugUtilsMessengerInfo(createInfo);



}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessenger::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

	return false;
}

void lvc::DebugUtilsMessenger::populateDebugUtilsMessengerInfo(
	VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT 
								| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
								| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
							| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
							| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}
