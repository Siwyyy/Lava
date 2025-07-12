#pragma once
#include "Object.h"

namespace Lava::ObjectSystem
{
	class CComponent : public Object
	{
	public:
		CComponent(const std::string& component_name_) : Object(component_name_) {}

		inline void setOwner(class AActor* owner_) { m_owner = owner_; }
		inline class AActor* getOwner() const { return m_owner; }

		inline void setActive(bool is_active_) { m_is_active = is_active_; }
		inline bool isActive() const { return m_is_active; }

		virtual void initialize() {}
		virtual void tick(float delta_time_) {}

		inline std::string getClassName() const override { return "Component"; }

	private:
		class AActor* m_owner = nullptr;
		bool m_is_active      = true;
	};
}
