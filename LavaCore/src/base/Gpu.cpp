#include "Lavapch.h"
#include "Gpu.h"

#include "Log.h"
#include "Window.h"

using namespace Lava;

Gpu::Gpu(const VkPhysicalDevice t_physical_device, const VkSurfaceKHR& t_surface)
	: m_physical_device(t_physical_device)
{
	VkPhysicalDeviceProperties device_properties;
	vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
	m_name.append_range(device_properties.deviceName);
	queryExtensions();
	checkExtensionSupport();
	findQueueFamilies(t_surface);

	rateDeviceSuitability();
}

void Gpu::logInfo() const
{
	LAVA_CORE_INFO("SELECTED GPU: {0}", m_name);
}

void Gpu::logFullInfo() const
{
	LAVA_CORE_DEBUG("GPU: {0}", m_name);
	LAVA_CORE_DEBUG("Score: {0}", m_score);
	LAVA_CORE_DEBUG("Device extensions:");
	for (const auto& required : required_extensions)
	{
		bool found = false;
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
}

void Gpu::queryExtensions()
{
	uint32_t extension_count;
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, nullptr);
	std::vector<VkExtensionProperties> device_extensions(extension_count);
	vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, device_extensions.data());

	for (const auto& extension : device_extensions)
	{
		const std::string* name = new std::string(extension.extensionName);
		m_available_extensions.emplace_back(name->c_str());
	}
}

void Gpu::checkExtensionSupport()
{
	uint32_t found = 0;
	for (const auto& required : required_extensions)
		for (const auto& available : m_available_extensions)
			if (!strcmp(required, available))
			{
				found++;
			}

	m_extensions_good = found == required_extensions.size();
}

void Gpu::rateDeviceSuitability()
{
	VkPhysicalDeviceProperties device_properties;
	VkPhysicalDeviceFeatures device_features;
	vkGetPhysicalDeviceProperties(m_physical_device, &device_properties);
	vkGetPhysicalDeviceFeatures(m_physical_device, &device_features);

	if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		m_score += 1000;
	m_score += static_cast<int>(device_properties.limits.maxImageDimension2D);

	if (!device_features.geometryShader
			|| !m_indices.isComplete()
			|| !m_extensions_good)
		m_score = 0;
}

void Gpu::findQueueFamilies(const VkSurfaceKHR& t_surface)
{
	// Get queue families
	uint32_t family_count;
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, nullptr);
	std::vector<VkQueueFamilyProperties> families(family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &family_count, families.data());

	// iterate through families until one that supports requirements is found
	bool found = false;
	for (int i = 0; !found && i < families.size(); i++)
	{
		const auto& family = families[i];

		//  Check for graphics support
		if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			m_indices.graphics = i;

		// Check for t_surface presentation support
		VkBool32 present_support = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, i, t_surface, &present_support);
		if (present_support)
			m_indices.present = i;

		found = m_indices.isComplete();
	}
}
