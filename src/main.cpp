#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

#include "window.hpp"

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    auto window = WindowBuilder()
                    .dimensions(1024, 768)
                    .title("Hello merely3d!")
                    .build();

    while (!window.should_close())
    {
        window.render_frame([] (Frame & frame)
        {
            frame.draw_box(Vector3(1.0, 1.0, 1.0), Position::Zero(), Orientation::Identity());
        });
    }

    glfwTerminate();
    return 0;
}
