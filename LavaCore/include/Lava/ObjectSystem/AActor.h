#pragma once
#include "Object.h"

namespace Lava::ObjectSystem
{
	template <typename T>
	concept is_component = std::is_base_of_v<class CComponent, T>;

	class AActor : public Object
	{
	public:
		AActor(const std::string& actor_name_) : Object(actor_name_) {}

		inline std::string getClassName() const override { return "Actor"; }

		inline bool isActive() const { return m_is_active; }

		// template <typename T, typename... Args>
		// T* addComponent(Args... args_);
		// template <typename T>
		// T* getComponent() const;

		template <is_component ComponentT, typename... Args>
		ComponentT* addComponent(Args... args_)
		{
			std::unique_ptr<ComponentT> component = std::make_unique<ComponentT>(std::forward<Args>(args_)...);
			ComponentT* component_ptr             = component.get();
			component_ptr->setOwner(this);
			m_components.push_back(std::move(component));
			component_ptr->initialize();
			return component_ptr;
		}

		template <is_component ComponentT>
		ComponentT* getComponent() const
		{
			for (const auto& component : m_components)
				if (ComponentT* casted_component = dynamic_cast<ComponentT*>(component.get()))
					return casted_component;
			return nullptr;
		}

		virtual void initialize() {}
		virtual void tick(float delta_time_);

	private:
		bool m_is_active = true;
		std::vector<std::unique_ptr<class CComponent>> m_components;
	};
}
