#pragma once
#include "Object.h"

namespace Lava::ObjectSystem
{
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

		template <typename T, typename... Args>
		T* addComponent(Args... args_)
		{
			static_assert(std::is_base_of_v<CComponent, T>, "T must be a subclass of Component");

			std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args_)...);
			T* component_ptr             = component.get();
			component_ptr->setOwner(this);
			m_components.push_back(std::move(component));
			component_ptr->initialize();
			return component_ptr;
		}

		template <typename T>
		T* getComponent() const
		{
			for (const auto& component : m_components)
				if (T* casted_component = dynamic_cast<T*>(component.get()))
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
