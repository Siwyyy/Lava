#pragma once

#include <optional>

namespace lvc
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		bool isComplete() const { return graphics_family.has_value() && present_family.has_value(); }
	};
}
