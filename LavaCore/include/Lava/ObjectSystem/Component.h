#pragma once
#include "Object.h"

namespace Lava::ObjectSystem
{
	class Component : public Object
	{
	public:
		Component(const std::string& component_name_) : Object(component_name_) {}

		inline void setOwner(class Actor* owner_) { m_owner = owner_; }
		inline class Actor* getOwner() const { return m_owner; }

		inline void setActive(bool is_active_) { m_is_active = is_active_; }
		inline bool isActive() const { return m_is_active; }

		virtual void initialize() = 0;
		virtual void tick(float delta_time_) = 0;

		inline std::string getClassName() const override { return "Component"; }

	private:
		class Actor* m_owner = nullptr;
		bool m_is_active     = true;
	};
}
