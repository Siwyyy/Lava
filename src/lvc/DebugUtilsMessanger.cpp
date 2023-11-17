#include "lvc/DebugUtilsMessanger.h"

#include <iostream>

using namespace lvc;

VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessanger::debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';

	return false;
}
