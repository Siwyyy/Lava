#pragma once

#include "Core.h"

namespace Lava
{
	class LAVA_API Input
	{
	public:
		Input()  = default;
		~Input() = default;

		bool isKeyPressed(uint32_t keycode_);

	private:
	};
}
