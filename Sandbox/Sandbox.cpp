#include "Lava.h"

class Sandbox final : public Lava::Application
{
public:
	Sandbox()  = default;
	~Sandbox() = default;
};

Lava::Application* Lava::createApplication()
{
	return new Sandbox;
}
