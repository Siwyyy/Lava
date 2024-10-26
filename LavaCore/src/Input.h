#pragma once

#include "Core.h"

#include <cstdint>

namespace Lava
{
	class LAVA_API Input
	{
	public:
		Input()  = default;
		virtual ~Input() = default;

		virtual bool isKeyPressed(uint32_t keycode_) = 0;

	private:
	};
}
