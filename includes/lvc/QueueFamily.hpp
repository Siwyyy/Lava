#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace lvc
{
	class QueueFamily
	{
	public:
		std::optional<uint32_t> graphicsFamily;

		inline bool isComplete() const { return graphicsFamily.has_value(); }
		static QueueFamily FindQueueFamilies(const VkPhysicalDevice& device);
	};
}