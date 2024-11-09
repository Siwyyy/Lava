#include "Lava.h"
#include "Lava/EntryPoint.h"

#include "SceneLayer.h"

class Sandbox final : public Lava::Application
{
public:
	Sandbox()           = default;
	~Sandbox() override = default;

private:
	void initResources() override
	{
		Lava::Resources::setDir(Lava::ResourceDir::Shaders, "./resources/shaders");
	}

	void initApp() override
	{
		pushLayer(new SceneLayer());
	}
};

Lava::Application* Lava::createApplication()
{
	return new Sandbox;
}
