#pragma once

#include "DebugMessenger.hpp"
#include "Gpu.hpp"

#include "vulkan/vulkan.h"

namespace lvc::inits
{
	constexpr const char* APP_NAME    = "LavaCore";
	constexpr uint32_t APP_VERSION    = VK_MAKE_API_VERSION(0, 0, 1, 0);
	constexpr const char* ENGINE_NAME = "Wrrrum";
	constexpr uint32_t ENGINE_VERSION = VK_MAKE_API_VERSION(0, 0, 1, 0);

	inline VkApplicationInfo applicationInfo()
	{
		VkApplicationInfo app_info;
		app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext              = nullptr;
		app_info.pApplicationName   = APP_NAME;
		app_info.applicationVersion = APP_VERSION;
		app_info.pEngineName        = ENGINE_NAME;
		app_info.engineVersion      = ENGINE_VERSION;
		app_info.apiVersion         = VK_API_VERSION_1_3;
		return app_info;
	}

	inline VkInstanceCreateInfo instanceInfo()
	{
		VkApplicationInfo app_info = applicationInfo();
		VkInstanceCreateInfo instance_info;
		instance_info.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_info.pNext                   = nullptr;
		instance_info.flags                   = NULL;
		instance_info.pApplicationInfo        = &app_info;
		instance_info.enabledLayerCount       = 0; // optional
		instance_info.ppEnabledLayerNames     = nullptr; // optional
		instance_info.enabledExtensionCount   = 0; // required
		instance_info.ppEnabledExtensionNames = nullptr; // required
		return instance_info;
	}

	inline VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
		debug_create_info.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_create_info.pNext           = nullptr;
		debug_create_info.flags           = NULL;
		debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
																				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
																				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
																		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
																		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_create_info.pfnUserCallback = DebugMessenger::debugCallback;
		debug_create_info.pUserData       = nullptr;
		return debug_create_info;
	}

	inline VkDeviceQueueCreateInfo deviceQueueCreateInfo()
	{
		constexpr float priority = 1.0f;
		VkDeviceQueueCreateInfo queue_create_info;
		queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_create_info.pNext            = nullptr;
		queue_create_info.flags            = NULL;
		queue_create_info.queueFamilyIndex = NULL; // required
		queue_create_info.queueCount       = 1;
		queue_create_info.pQueuePriorities = &priority;
		return queue_create_info;
	}

	inline VkDeviceCreateInfo deviceCreateInfo()
	{
		VkDeviceCreateInfo device_create_info;
		device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pNext                   = nullptr;
		device_create_info.flags                   = NULL;
		device_create_info.queueCreateInfoCount    = NULL;    // required
		device_create_info.pQueueCreateInfos       = nullptr; // required
		device_create_info.enabledLayerCount       = 0; // optional
		device_create_info.ppEnabledLayerNames     = nullptr; // optional
		device_create_info.pEnabledFeatures        = nullptr;
		device_create_info.enabledExtensionCount   = static_cast<uint32_t>(Gpu::required_extensions.size());
		device_create_info.ppEnabledExtensionNames = Gpu::required_extensions.data();
		return device_create_info;
	}
}
