#include "window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace merely3d
{

void Window::render_frame_impl(Frame & frame)
{
    glfwMakeContextCurrent(_glfw_window.get());
    glClear(GL_COLOR_BUFFER_BIT);

    // TODO: Process resulting command buffer from Frame

    /* Swap front and back buffers */
    glfwSwapBuffers(_glfw_window.get());

    /* Poll for and process events */
    glfwPollEvents();
}

bool Window::should_close() const
{
    return glfwWindowShouldClose(_glfw_window.get());
}

Window WindowBuilder::build() const
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // This is apparently needed on Mac OS X. Can we simply set it for all platforms...?
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * glfw_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);

    if (!glfw_window)
    {
        // TODO: Better error, more information
        throw std::runtime_error("Failed to initialize GLFWwindow");
    }

    // TODO: Should glad initialization happen in a different location...?
    // Is it safe to re-initialize GLAD?
    glfwMakeContextCurrent(glfw_window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        // TODO: Better error message
        throw std::runtime_error("Failed to initialize GLAD");
    }

    auto window_ptr = Window::GlfwWindowPtr(glfw_window, glfwDestroyWindow);
    auto window = Window(std::move(window_ptr));
    return std::move(window);
}

}