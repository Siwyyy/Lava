#pragma once
#include "Events/Event.h"

namespace Lava
{
	class Layer
	{
	public:
		Layer()          = default;
		virtual ~Layer() = default;

		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
		virtual void onUpdate() = 0;
		virtual void onDraw() = 0;
		virtual void onEvent(Event& event_) = 0;
	};
}
