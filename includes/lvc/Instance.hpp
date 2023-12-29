#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "InstanceExtensions.hpp"

namespace lvc
{
	class Instance
	{
	public:
		Instance(const char* app_name, const char* engine_name);
		Instance() = delete;
		~Instance();

		inline const VkInstance& handle() const { return m_instance; }
		inline const InstanceExtensions& hInstanceExtensions() const { return m_instance_extensions; }

		static inline bool validationLayersEnabled() { return enable_validation_layers; }

		static const std::vector<const char*> validation_layers;

	private:
		VkInstance m_instance;
		InstanceExtensions m_instance_extensions;

		static const bool enable_validation_layers;

		static bool checkValidationLayerSupport();
	};
}
