#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace lvc
{
	class Instance
	{
	private:
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
