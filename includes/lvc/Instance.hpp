#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace lvc
{
	class Instance
	{
	public:
		Instance(const char* appName, const char* engineName, bool enableValidationLayers);
		Instance() = delete;
		~Instance();

		inline VkInstance& handle() { return m_instance; }

		static const std::vector<const char*> validationLayers;

	private:
		VkInstance m_instance;
		bool m_enableValidationLayers;

		static void getRequiredExtensions(std::vector<const char*> &extensions);
	};
}