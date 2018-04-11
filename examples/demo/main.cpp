#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>
#include <algorithm>

#include <merely3d/window.hpp>
#include <merely3d/camera_controller.hpp>

#include <Eigen/Geometry>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Key;
using merely3d::Action;
using merely3d::EventHandler;
using merely3d::Material;
using merely3d::Color;
using merely3d::CameraController;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;
using merely3d::Vector2;

using Eigen::Vector3f;

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
                    .multisampling(8)
                    .build();

        window.camera().look_in(Vector3f(1.0, 0.0, -1), Vector3f(0.0, 0.0, 1.0));
        window.camera().set_position(Vector3f(-1.0, 0.0, 3.0));

        window.add_event_handler(std::shared_ptr<EventHandler>(new CameraController));

        while (!window.should_close())
        {
            window.render_frame([] (Frame & frame)
            {
                const auto rect_material = Material().with_color(Color(0.5, 0.3, 0.3));
                Orientation orientation = Orientation::Identity();
                orientation = Eigen::AngleAxisf(0.78, Eigen::Vector3f(1.0, 0.0, 0.0));
                frame.draw_rectangle(Vector2(0.5, 0.5),
                                     Position(1.0, 0.0, 0.05),
                                     orientation,
                                     rect_material);
                frame.draw_box(Vector3(1.0, 1.0, 1.0), Position(4.0, 0.0, 0.1));

                frame.draw_box(Vector3(0.3, 5.0, 3.0), Position(0.0, 0.0, 0.0));
                frame.draw_box(Vector3(0.2, 1.0, 1.0),
                               Position(0.0, 0.0, 5.0),
                               Orientation(Eigen::AngleAxisf(0.5, Vector3(1.0, 1.0, 1.0))),
                               Material().with_color(Color(1.0, 0.0, 0.0)));
            });
        }
    }

    glfwTerminate();
    return 0;
}
