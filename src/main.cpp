#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

#include <merely3d/window.hpp>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;
using merely3d::Vector2;

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Scope is necessary because Window calls GL destructors in constructor
    {
        auto window = WindowBuilder()
                    .dimensions(1024, 768)
                    .title("Hello merely3d!")
                    .build();

        while (!window.should_close())
        {
            window.render_frame([] (Frame & frame)
            {
                Orientation orientation = Orientation::Identity();
                orientation = Eigen::AngleAxisf(0.78, Eigen::Vector3f(1.0, 0.0, 0.0));
                frame.draw_rectangle(Vector2(0.5, 0.5), Position(0.0, 0.0, -1.0), orientation);
            });
        }
    }

    glfwTerminate();
    return 0;
}
