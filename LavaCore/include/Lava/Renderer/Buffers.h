#pragma once
#include "Lava/Lavapch.h"

#include "Lava/Application.h"

namespace Lava::Buffers
{
	void createVulkanBuffer(VkDeviceSize size_, VkBufferUsageFlags usage_, VkMemoryPropertyFlags props_, VkBuffer& buffer_, VkDeviceMemory& buffer_memory_)
	{
		auto context = Lava::Application::getInstance().getWindow().getContext();

		VkBufferCreateInfo buffer_create_info;
		buffer_create_info.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.pNext                 = nullptr;
		buffer_create_info.flags                 = NULL;
		buffer_create_info.size                  = size_;
		buffer_create_info.usage                 = usage_;
		buffer_create_info.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
		buffer_create_info.queueFamilyIndexCount = 0;
		buffer_create_info.pQueueFamilyIndices   = nullptr;

		if (vkCreateBuffer(context->getDevice(), &buffer_create_info, nullptr, &buffer_) != VK_SUCCESS)
			LAVA_CORE_ERROR("failed to create vertex buffer!");

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(context->getDevice(), buffer_, &memory_requirements);

		uint32_t memory_type_index = 0;
		bool found                 = false;
		VkPhysicalDeviceMemoryProperties properties;
		vkGetPhysicalDeviceMemoryProperties(context->getGpu(), &properties);
		for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
			if ((memory_requirements.memoryTypeBits & (1 << i)) && (properties.memoryTypes[i].propertyFlags & props_) == props_)
			{
				memory_type_index = i;
				found             = true;
			}
		LAVA_ASSERT(found, "Failed to find memory type!")

		VkMemoryAllocateInfo allocate_info;
		allocate_info.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocate_info.pNext           = nullptr;
		allocate_info.allocationSize  = memory_requirements.size;
		allocate_info.memoryTypeIndex = memory_type_index;

		if (vkAllocateMemory(context->getDevice(), &allocate_info, nullptr, &buffer_memory_) != VK_SUCCESS)
			LAVA_CORE_ERROR("failed to allocate vertex buffer memory!");

		vkBindBufferMemory(context->getDevice(), buffer_, buffer_memory_, 0);
	}
}
