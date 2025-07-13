#include "Lava/LavaPCH.h"
#include "Lava/ObjectSystem/AActor.h"

#include "Lava/ObjectSystem/CComponent.h"

using namespace Lava::ObjectSystem;

void AActor::tick(float delta_time_)
{
	if (!m_is_active)
		return;

	for (auto& component : m_components)
	{
		if (component->isActive())
			component->tick(delta_time_);
	}
}
