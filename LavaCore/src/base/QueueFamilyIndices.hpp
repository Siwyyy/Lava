#pragma once

#include <optional>

namespace lvc
{
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> present;

		bool isComplete() const { return graphics.has_value() && present.has_value(); }
	};
}
