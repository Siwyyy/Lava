#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class DebugMessenger;
	class Window;
	class GpuManager;
	class Device;
	class Swapchain;

	class Instance
	{
	public:
		Instance(const char* t_app_name,
						 const char* t_engine_name);
		Instance() = delete;
		~Instance();

		inline const VkInstance& hVkInstance() const { return m_instance; }

		inline static bool validationLayersEnabled() { return validation_layers_enabled; }

		inline static const std::vector<const char*> validation_layers = {"VK_LAYER_KHRONOS_validation"};

	private:
		VkInstance m_instance;

		std::vector<const char*> m_available_extensions{};
		std::vector<const char*> m_required_extensions{};

		bool m_extensions_good = false;

		static const bool validation_layers_enabled;

		void setupExtensions();
		void queryExtensions();
		void setRequiredExtensions();
		void checkRequiredExtensions();
		void logAvailableExtensions() const;
		void logRequiredExtensions() const;

		static bool checkValidationLayerSupport();

		inline static const std::vector<const char*> default_extensions = {"VK_KHR_surface",
																																			 "VK_KHR_win32_surface",
																																			 "VK_KHR_device_group_creation"};
	};
}
