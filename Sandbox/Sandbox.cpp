#include "Lava.hpp"

class Sandbox final : public lava::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

lava::Application* lava::createApplication()
{
	return new Sandbox;
}
