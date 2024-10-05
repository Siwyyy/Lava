#pragma once

#ifdef LAVA_PLATFORM_WINDOWS

extern lava::Application* lava::createApplication();

int main(int argc, char** argv)
{
	try
	{
		lava::Log::init();
		LAVA_CORE_INFO("Logger initialized!");

		const auto app = lava::createApplication();
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
