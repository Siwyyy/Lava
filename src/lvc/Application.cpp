#include "lvc/Application.hpp"

using namespace lvc;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

lvc::Application::Application()
    : window(WIDTH,HEIGHT,"LavaCore - Test"),
      instance("LavaCore","No Engine", enableValidationLayers)
{}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window.window()))
    {
        glfwPollEvents();
    }
}