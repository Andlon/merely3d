#include "window.hpp"

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

}