#pragma once
#include "Lava/Lavapch.h"

namespace Lava::Buffers
{
	void createVulkanBuffer(VkDeviceSize size_,
													VkBufferUsageFlags usage_,
													VkMemoryPropertyFlags props_,
													VkBuffer& buffer_,
													VkDeviceMemory& buffer_memory_);
}
