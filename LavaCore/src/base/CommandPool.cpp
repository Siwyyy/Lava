#include "CommandPool.hpp"

#include "Gpu.hpp"
#include "Log.hpp"

using namespace lava;

CommandPool::CommandPool(const VkDevice& t_device, const QueueFamilyIndices& t_indices)
	: m_device(t_device)
	, m_indices(t_indices)
{
	VkCommandPoolCreateInfo command_pool_create_info;
	command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.queueFamilyIndex = m_indices.graphics.value();
	command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.pNext            = nullptr;

	if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_pool) != VK_SUCCESS)
		LAVA_CORE_ERROR("Failed to create command pool!");
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(m_device, m_pool, nullptr);
}
