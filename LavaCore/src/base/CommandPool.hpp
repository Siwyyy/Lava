#pragma once

#include "Device.hpp"

namespace lava
{
	class Device;
	struct QueueFamilyIndices;

	class CommandPool
	{
	public:
		CommandPool(const VkDevice& t_device,
								const QueueFamilyIndices& t_indices);
		CommandPool() = delete;
		~CommandPool();

		inline const VkCommandPool& hCommandPool() const { return m_pool; }

	private:
		const VkDevice& m_device;
		const QueueFamilyIndices& m_indices;

		VkCommandPool m_pool;
	};
}
