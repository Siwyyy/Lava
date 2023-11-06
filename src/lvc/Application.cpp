#include "lvc/Application.hpp"

using namespace lvc;

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

lvc::Application::Application()
    : window(WIDTH,HEIGHT,"LavaCore - Test")
    , instance("LavaCore","No Engine")
{}

void Application::mainLoop()
{
    while (!glfwWindowShouldClose(window.handle()))
        glfwPollEvents();
}