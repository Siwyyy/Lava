#include "Lava.h"

class Sandbox final : public Lava::Application
{
public:
	Sandbox() {}
	~Sandbox() {}
};

Lava::Application* Lava::createApplication()
{
	return new Sandbox;
}
