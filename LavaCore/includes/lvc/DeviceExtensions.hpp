#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class DeviceExtensions
	{
	public:
		explicit DeviceExtensions(const VkPhysicalDevice* physical);
		DeviceExtensions()  = delete;
		~DeviceExtensions() = default;

		inline bool isGood() const { return m_is_good; }

		void logDeviceExtensions() const;
		void logRequiredExtensions() const;

		static const std::vector<const char*> required_extension_names;

	private:
		const VkPhysicalDevice* m_physical;
		std::vector<VkExtensionProperties> m_available_extensions;
		bool m_is_good;

		void enumerateDeviceExtensions();
	};
}
