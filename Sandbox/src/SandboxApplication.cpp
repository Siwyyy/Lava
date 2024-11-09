#include "Lava.h"
#include "Lava/EntryPoint.h"

#include "Lava/Resources.h"

#include "SceneLayer.h"

class Sandbox final : public Lava::Application
{
public:
	Sandbox()           = default;
	~Sandbox() override = default;

private:
	void initResources() override
	{
		Lava::Resources::setDir(Lava::ResourceDir::Shaders, "./resources/Shaders");
		Lava::Resources::setDir(Lava::ResourceDir::Models, "./resources/Models");
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
