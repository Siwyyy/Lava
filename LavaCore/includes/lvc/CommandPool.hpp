#pragma once

#include "lvc/Device.hpp"

namespace lvc
{
	class Device;
	class QueueFamilyIndices;

	class CommandPool
	{
	public:
		explicit CommandPool(const Device* t_device);
		~CommandPool();

		inline const VkCommandPool& hCommandPool() const { return m_pool; }

	private:
		const VkDevice& m_device;
		const QueueFamilyIndices* m_indices;

		VkCommandPool m_pool;
	};
}
