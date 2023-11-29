#include "lvc/Application.hpp"

using namespace lvc;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

lvc::Application::Application()
    : instance("LavaCore", "No Engine")
    , debugMessenger(instance)
    , window(WIDTH,HEIGHT,"LavaCore - Test")
{}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window.handle()))
        glfwPollEvents();
}