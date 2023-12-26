#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.h>

namespace lvc
{
    class DebugUtilsMessenger
    {
    public:
        explicit DebugUtilsMessenger(const Instance* instance);

        ~DebugUtilsMessenger() noexcept(false);

        inline VkDebugUtilsMessengerEXT handle() const { return m_debugMessenger; }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
            void* p_user_data);
        static void populateDebugUtilsMessengerInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    private:
        VkDebugUtilsMessengerEXT m_debugMessenger;
        const Instance* m_instance;

        VkResult createDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
            const VkAllocationCallbacks* p_allocator);
        void destroyDebugUtilsMessengerExt(const VkAllocationCallbacks* p_allocator) const;
    };
}
