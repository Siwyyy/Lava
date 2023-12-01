#include "lvc/DebugUtilsMessenger.hpp"

#include <iostream>

using namespace lvc;

lvc::DebugUtilsMessenger::DebugUtilsMessenger(const Instance& instance)
	: m_debugMessanger(VK_NULL_HANDLE)
	, m_instance(instance)
{
	if (!m_instance.validationLayersEnabled())
		return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	populateDebugUtilsMessengerInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(&createInfo, nullptr) != VK_SUCCESS)
		throw std::runtime_error("Failed to setup Debug Utils Messenger");
}

lvc::DebugUtilsMessenger::~DebugUtilsMessenger() noexcept(false)
{
	if (!m_instance.validationLayersEnabled())
		return;

	DestroyDebugUtilsMessengerEXT(nullptr);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessenger::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "[validation layer]\t" << pCallbackData->pMessage << "\n";

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

VkResult lvc::DebugUtilsMessenger::CreateDebugUtilsMessengerEXT(
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator)
{
	auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
		m_instance.handle(), "vkCreateDebugUtilsMessengerEXT"
	);

	if (createFunc != nullptr)
		return createFunc(m_instance.handle(), pCreateInfo, pAllocator, &m_debugMessanger);
	else
		return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void lvc::DebugUtilsMessenger::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator)
{
	auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		m_instance.handle(), "vkDestroyDebugUtilsMessengerEXT");

	if (destroyFunc != nullptr)
		destroyFunc(m_instance.handle(), m_debugMessanger, pAllocator);
	else
		throw std::runtime_error("Failed to destroy Debug Utils Messenger");
}
