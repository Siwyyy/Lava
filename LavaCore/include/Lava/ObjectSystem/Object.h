#pragma once

namespace Lava::ObjectSystem
{
	class Object
	{
	public:
		Object() = delete;

		Object(const std::string& object_name_) : m_name(object_name_) {}

		virtual ~Object() = default;

		const std::string& getName() const { return m_name; }
		virtual std::string getClassName() const { return "Object"; }

	private:
		std::string m_name;
	};
}
