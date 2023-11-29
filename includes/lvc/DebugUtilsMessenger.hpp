#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.h>

namespace lvc
{
    class DebugUtilsMessenger
    {
    public:
        explicit DebugUtilsMessenger(const Instance& instance);
        ~DebugUtilsMessenger() noexcept(false);

        inline const VkDebugUtilsMessengerEXT handle() const { return m_debugMessanger; }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);

    private:
        VkDebugUtilsMessengerEXT m_debugMessanger;
        const Instance& m_instance;

        void populateDebugUtilsMessengerInfo(
            VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    };
}
