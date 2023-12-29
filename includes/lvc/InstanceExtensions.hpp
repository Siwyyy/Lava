#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class InstanceExtensions
	{
	public:
		InstanceExtensions();
		~InstanceExtensions() = default;

		inline const std::vector<VkExtensionProperties>& available() const { return m_available_extensions; }
		inline const std::vector<const char*>& required() const { return m_required_extension_names; }

	private :
		std::vector<VkExtensionProperties> m_available_extensions;
		std::vector<const char*> m_required_extension_names;

		void enumerateInstanceExtensions();
		void getRequiredGlfwExtensions();
		void checkRequiredExtensions() const;
	};
}
