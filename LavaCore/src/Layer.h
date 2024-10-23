#pragma once
#include "Events/Event.h"

namespace Lava
{
	class Layer
	{
	public:
		Layer();
		virtual ~Layer();

		virtual void onAttach() = 0;
		virtual void onDetach() = 0;
		virtual void onUpdate() = 0;
		virtual void onEvent(Event& event_) = 0;
	};
}
