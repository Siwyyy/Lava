#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.h>

namespace lvc
{
    class DebugMessenger
    {
    public:
        explicit DebugMessenger(Instance* instance);

        ~DebugMessenger() noexcept(false);

        inline VkDebugUtilsMessengerEXT handle() const { return m_debug_messenger; }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
            void* p_user_data);
        static void populateDebugUtilsMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
				Instance* m_instance;
				VkDebugUtilsMessengerEXT m_debug_messenger;

				VkResult createDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
            const VkAllocationCallbacks* p_allocator);
        void destroyDebugUtilsMessengerExt(const VkAllocationCallbacks* p_allocator) const;
    };
}
