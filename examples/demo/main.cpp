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
using merely3d::Line;

using merely3d::red;

using merely3d::renderable;
using merely3d::Rectangle;
using merely3d::Box;
using merely3d::Sphere;

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Quaternionf;
using Eigen::AngleAxisf;

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
            frame.draw(renderable(Rectangle(0.5, 0.5))
                        .with_position(1.0, 0.0, 0.5)
                        .with_orientation(AngleAxisf(0.78, Vector3f(1.0f, 0.0f, 0.0f)))
                        .with_material(Material().with_color(Color(0.5, 0.3, 0.3))));

            frame.draw(renderable(Box(1.0, 1.0, 1.0))
                        .with_position(4.0, 0.0, 1.1));

            frame.draw(renderable(Box(0.2, 5.0, 1.0))
                        .with_position(0.0, 0.0, 1.0));

            frame.draw(renderable(Box(0.2, 1.0, 1.0))
                        .with_position(0.0, 0.0, 5.0)
                        .with_orientation(AngleAxisf(0.5, Vector3f(1.0, 1.0, 1.0)))
                        .with_material(Material().with_color(red())));

            frame.draw(renderable(Sphere(1.0))
                        .with_position(3.0, 3.0, 3.0));

            frame.draw(renderable(Sphere(1.0))
                        .with_position(3.0, 3.0, 6.0)
                        .with_material(Material().with_wireframe(true)));

            frame.draw(renderable(Box(1.0, 2.0, 1.0))
                        .with_position(-3.0, 3.0, 9.0)
                        .with_material(Material().with_wireframe(true)));

            frame.draw_line(Line(Vector3f(0.0, 0.0, 0.0), Vector3f(10.0, -5.0, 10.0)));

            const auto floor_color = Color(0.5f, 0.35f, 0.35f);
            frame.draw(renderable(Rectangle(20.0f, 20.0f))
                               .with_position(0.0f, 0.0f, 0.0f)
                               .with_material(Material().with_color(floor_color)));
        });
    }

    return 0;
}
