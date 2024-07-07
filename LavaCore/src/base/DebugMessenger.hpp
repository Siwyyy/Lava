#pragma once

#include <vulkan/vulkan.h>

namespace lvc
{
	class DebugMessenger
	{
	public:
		explicit DebugMessenger(const VkInstance& instance);

		~DebugMessenger() noexcept(false);

		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_type,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
			void* p_user_data);

		static VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo();

	private:
		//OutRefs
		const VkInstance& m_instance;

		//Members
		VkDebugUtilsMessengerEXT m_debug_messenger;

		VkResult createDebugUtilsMessengerExt(
			const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
			const VkAllocationCallbacks* p_allocator);
		void destroyDebugUtilsMessengerExt(const VkAllocationCallbacks* p_allocator) const;
	};
}
