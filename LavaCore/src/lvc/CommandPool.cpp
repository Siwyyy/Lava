#include "lvc/CommandPool.hpp"

#include "lvc/PhysicalDevice.hpp"

#include <iostream>
#include <stdexcept>

using namespace lvc;

CommandPool::CommandPool(const Device* t_device)
	: m_device(t_device->hDevice())
	, m_indices(t_device->physicalDevice()->indices())
{
	VkCommandPoolCreateInfo command_pool_create_info;
	command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.queueFamilyIndex = m_indices->graphics_family.value();
	command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	command_pool_create_info.pNext            = nullptr;

	if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &m_pool) != VK_SUCCESS)
		throw std::runtime_error("err: Failed to create command pool!\n");
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(m_device, m_pool, nullptr);
	std::clog << "Successfully destroyed command pool\n";
}