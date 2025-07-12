#include "Lava/Lavapch.h"
#include "Lava/ObjectSystem/Actor.h"

#include "Lava/ObjectSystem/Component.h"

using namespace Lava::ObjectSystem;

void Actor::tick(float delta_time_)
{
	if (!m_is_active)
		return;

	for (auto& component : m_components)
	{
		if (component->isActive())
			component->tick(delta_time_);
	}
}
