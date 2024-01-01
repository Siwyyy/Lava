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

		inline const std::vector<VkExtensionProperties>& available() const { return m_available_extensions; }
		inline const std::vector<const char*>& required() const { return m_required_extension_names; }
		inline bool isGood() const { return m_is_good; }

	private:
		const VkPhysicalDevice* m_physical;
		std::vector<VkExtensionProperties> m_available_extensions;
		std::vector<const char*> m_required_extension_names;
		bool m_is_good;

		void enumerateDeviceExtensions();
		bool checkRequiredExtensions() const;
	};
}
