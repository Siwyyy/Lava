#pragma once

#include "Instance.hpp"

#include <vulkan/vulkan.h>

namespace lvc
{
    class DebugUtilsMessanger
    {
    public:
        inline const VkDebugUtilsMessengerEXT handle() const { return m_debugMessanger; }

        static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData);
    private:
        const Instance m_instance;
        VkDebugUtilsMessengerEXT m_debugMessanger;

    };
}
