#include "lvc/QueueFamily.hpp"

#include <vector>

using namespace lvc;

QueueFamily QueueFamily::FindQueueFamilies(const VkPhysicalDevice& device)
{
    QueueFamily indices;

    // Get queue families
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount,families.data());

    // iterate through families until one that supports requirements is found
    bool found = false;
    for (int i = 0; !found && i < families.size(); i++)
    {
        const auto& family = families[i];

        //  Check for graphics support
        if (family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        found = indices.isComplete();
    }

    return indices;
}
