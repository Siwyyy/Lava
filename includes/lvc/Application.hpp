#pragma once

#include "lvc/Instance.hpp"
#include "lvc/DebugUtilsMessenger.hpp"
#include "lvc/Window.hpp"

#include <vector>

namespace lvc
{
	class Application
	{
	public:
		Application();

		void run() { mainLoop(); }

	private:
		lvc::Instance instance;
		lvc::Window window;
		lvc::DebugUtilsMessenger debugMessenger;

		void mainLoop();
	};
}

