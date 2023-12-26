#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace lvc
{
	struct QueueFamilyIndices
	{
	public:
		std::optional<uint32_t> graphics_family;

		inline bool isComplete() const { return graphics_family.has_value(); }
	};
}
