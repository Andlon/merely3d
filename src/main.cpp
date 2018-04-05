#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

#include <merely3d/window.hpp>

#include <Eigen/Geometry>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Key;
using merely3d::KeyAction;
using merely3d::KeyListener;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;
using merely3d::Vector2;

using Eigen::Vector3f;

struct CameraKeyController : public merely3d::KeyListener
{
    virtual bool key_press(Window & window,
                           Key key,
                           KeyAction action,
                           int scancode,
                           int modifiers) override
    {
        using Eigen::AngleAxisf;

        const auto STRAFE_STEP_LENGTH = 0.5;
        const auto ANGLE_STEP = 0.05;
        auto & camera = window.camera();

        Eigen::Vector3f strafe_direction = Eigen::Vector3f::Zero();
        const auto x = Eigen::Vector3f(1.0, 0.0, 0.0);
        const auto y = Eigen::Vector3f(0.0, 1.0, 0.0);
        const auto z = Eigen::Vector3f(0.0, 0.0, 1.0);

        Eigen::Vector3f rotation_axis = Eigen::Vector3f::Zero();

        if (action == KeyAction::Press || action == KeyAction::Repeat)
        {
            switch (key)
            {
                case Key::W: strafe_direction += camera.direction(); break;
                case Key::S: strafe_direction -= camera.direction(); break;
                case Key::A: strafe_direction -= camera.right(); break;
                case Key::D: strafe_direction += camera.right(); break;
                case Key::Space: strafe_direction += camera.up(); break;
                case Key::C: strafe_direction -= camera.up(); break;
                case Key::Left: rotation_axis += z; break;
                case Key::Right: rotation_axis -= z; break;
                case Key::Up: rotation_axis += camera.right(); break;
                case Key::Down: rotation_axis -= camera.right(); break;
            }

            strafe_direction.normalize();
            rotation_axis.normalize();

            const AngleAxisf rotation = AngleAxisf(ANGLE_STEP, rotation_axis);
            const Eigen::Quaternionf new_orientation = rotation * camera.orientation();

            camera.set_orientation(new_orientation);
            camera.set_position(camera.position() + STRAFE_STEP_LENGTH * strafe_direction);
        }
    }
};

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

        window.camera().look_in(Vector3f(1.0, 0.0, -1), Vector3f(0.0, 0.0, 1.0));
        window.camera().set_position(Vector3f(-1.0, 0.0, 3.0));

        window.add_key_listener(std::shared_ptr<KeyListener>(new CameraKeyController()));

        while (!window.should_close())
        {
            window.render_frame([] (Frame & frame)
            {
                Orientation orientation = Orientation::Identity();
                orientation = Eigen::AngleAxisf(0.78, Eigen::Vector3f(1.0, 0.0, 0.0));
                frame.draw_rectangle(Vector2(0.5, 0.5), Position(1.0, 0.0, 0.05), orientation);
                frame.draw_box(Vector3(1.0, 1.0, 1.0), Position(4.0, 0.0, 0.1));
            });
        }
    }

    glfwTerminate();
    return 0;
}
