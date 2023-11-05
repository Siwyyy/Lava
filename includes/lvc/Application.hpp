#pragma once

#include "lvc/Window.hpp"
#include "lvc/Instance.hpp"

#include <vector>

namespace lvc
{
	class Application
	{
	public:
		Application();

		void run() { mainLoop(); }

	private:
		lvc::Window window;
		lvc::Instance instance;

		void mainLoop();
	};
}

