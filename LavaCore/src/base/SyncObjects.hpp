#pragma once

#include <vulkan/vulkan_core.h>

#include <vector>

namespace lava
{
	class SyncObjects
	{
	public:
		SyncObjects(const VkDevice& t_device, uint32_t t_max_frames_in_flight);
		~SyncObjects();

		inline const VkSemaphore& hSemaphoreImageAvailable(const uint32_t index) const { return m_semaphore_image_available[index]; }
		inline const VkSemaphore& hSemaphoreRenderFinished(const uint32_t index) const { return m_semaphore_render_finished[index]; }
		inline const VkFence& hFenceInFlight(const uint32_t index) const { return m_fence_in_flight[index]; }

	private:
		const VkDevice& m_device;
		const int m_max_frames_in_flight;

		std::vector<VkSemaphore> m_semaphore_image_available;
		std::vector<VkSemaphore> m_semaphore_render_finished;
		std::vector<VkFence> m_fence_in_flight;
	};
}
