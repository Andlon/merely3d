#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>
#include <algorithm>

#include <merely3d/app.hpp>
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

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Quaternionf;

int main()
{
    // Constructing the app first is essential: it makes sure that
    // GLFW is set up properly. Note that as an alternative, you can call
    // glfw init/terminate yourself directly, but you must be careful that
    // any windows are destroyed before calling terminate(). App automatically
    // takes care of this as long as it outlives any windows.
    merely3d::App app;

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
            Quaternionf orientation = Quaternionf::Identity();
            orientation = Eigen::AngleAxisf(0.78, Vector3f(1.0f, 0.0f, 0.0f));
            frame.draw_rectangle(Vector2f(0.5f, 0.5f),
                                 Vector3f(1.0f, 0.0f, 0.05f),
                                 orientation,
                                 rect_material);
            frame.draw_box(Vector3f(1.0f, 1.0f, 1.0f), Vector3f(4.0f, 0.0f, 0.1f));

            frame.draw_box(Vector3f(0.3f, 5.0f, 3.0f), Vector3f(0.0f, 0.0f, 0.0f));
            frame.draw_box(Vector3f(0.2f, 1.0f, 1.0f),
                           Vector3f(0.0f, 0.0f, 5.0f),
                           Quaternionf(Eigen::AngleAxisf(0.5f, Vector3f(1.0f, 1.0f, 1.0f))),
                           Material().with_color(Color(1.0, 0.0, 0.0)));
        });
    }

    return 0;
}
