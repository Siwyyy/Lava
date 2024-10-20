#pragma once

#ifdef LAVA_PLATFORM_WINDOWS

extern Lava::Application* Lava::createApplication();

int main(int argc, char** argv)
{
	try
	{
		Lava::Log::init();
		LAVA_CORE_INFO("Logger initialized!");

		const auto app = Lava::createApplication();
		app->run();
		delete app;
	}
	catch (const std::exception& e)
	{
		LAVA_CORE_ERROR(e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif
