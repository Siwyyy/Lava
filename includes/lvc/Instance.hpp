#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace lvc
{
	class Instance
	{
	public:
		Instance(const char* app_name, const char* engine_name);
		Instance() = delete;
		~Instance();

		inline const VkInstance& handle() const { return m_instance; }
		static inline bool validationLayersEnabled() { return enable_validation_layers; }
		static const std::vector<const char*> validation_layers;
		static const std::vector<const char*> device_extensions;

	private:
		VkInstance m_instance;
		static const bool enable_validation_layers;

		static bool checkValidationLayerSupport();
		static void getRequiredExtensions(std::vector<const char*> &extensions);
	};
}