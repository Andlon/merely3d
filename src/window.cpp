#include "window.hpp"

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
    // TODO: Throw exception if creating the window fails (i.e. returns NULL)
    GLFWwindow * glfw_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);
    auto window_ptr = Window::GlfwWindowPtr(glfw_window, glfwDestroyWindow);
    auto window = Window(std::move(window_ptr));
    return std::move(window);
}

}