#include "lvc/Application.hpp"

#include <iostream>

int main()
{
	try
	{
		const lvc::Application app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
