#include <merely3d/app.hpp>

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

namespace merely3d
{
    static void error_callback(int error, const char * description)
    {
        std::cerr << "GLFW error code " << error << ". Description" << description << std::endl;
    }

    App::App()
    {
        glfwSetErrorCallback(error_callback);
        if (!glfwInit())
        {
            throw std::runtime_error("Fatal error: Could not initialize the GLFW library.");
        }
    }

    App::~App()
    {
        glfwTerminate();
    }

}