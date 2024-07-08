#pragma once
#include <iostream>

#ifdef LAVA_PLATFORM_WINDOWS

extern lava::Application* lava::createApplication();

int main(int argc, char** argv)
{
	try
	{
		auto app = lava::createApplication();
		app->run();
		delete app;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#endif
