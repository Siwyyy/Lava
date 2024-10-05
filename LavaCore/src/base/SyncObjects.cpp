#include "SyncObjects.hpp"

#include "Log.hpp"

using namespace lava;

SyncObjects::SyncObjects(const VkDevice& t_device, const uint32_t t_max_frames_in_flight)
	: m_device(t_device)
	, m_max_frames_in_flight(t_max_frames_in_flight)
{
	m_semaphore_image_available.resize(m_max_frames_in_flight);
	m_semaphore_render_finished.resize(m_max_frames_in_flight);
	m_fence_in_flight.resize(m_max_frames_in_flight);

	VkSemaphoreCreateInfo semaphore_create_info;
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_create_info.flags = 0;
	semaphore_create_info.pNext = nullptr;

	VkFenceCreateInfo fence_create_info;
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	fence_create_info.pNext = nullptr;

	for (size_t i = 0; i < m_max_frames_in_flight; i++)
	{
		if (vkCreateSemaphore(t_device, &semaphore_create_info, nullptr, &m_semaphore_image_available[i]) != VK_SUCCESS ||
				vkCreateSemaphore(t_device, &semaphore_create_info, nullptr, &m_semaphore_render_finished[i]) != VK_SUCCESS ||
				vkCreateFence(t_device, &fence_create_info, nullptr, &m_fence_in_flight[i]) != VK_SUCCESS)
			LAVA_CORE_ERROR("Failed to create semaphores/fence!");
	}
}

SyncObjects::~SyncObjects()
{
	for (size_t i = 0; i < m_max_frames_in_flight; i++)
	{
		vkDestroySemaphore(m_device, m_semaphore_image_available[i], nullptr);
		vkDestroySemaphore(m_device, m_semaphore_render_finished[i], nullptr);
		vkDestroyFence(m_device, m_fence_in_flight[i], nullptr);
	}
}
