#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace lvc
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		inline bool isComplete() const { return graphicsFamily.has_value(); }
	};

	class QueueFamily
	{
	public:
		QueueFamily() = delete;
		~QueueFamily() = delete;

		static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
	};
}