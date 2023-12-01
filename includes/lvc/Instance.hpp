#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace lvc
{
	class Instance
	{
	public:
		Instance(const char* appName, const char* engineName);
		Instance() = delete;
		~Instance();

		inline const VkInstance& handle() const { return m_instance; }
		inline bool validationLayersEnabled() const { return m_enableValidationLayers; }

	private:
		VkInstance m_instance;
		static const bool m_enableValidationLayers;
		static const std::vector<const char*> m_validationLayers;

		static bool checkValidationLayerSupport();
		static void getRequiredExtensions(std::vector<const char*> &extensions);
	};
}