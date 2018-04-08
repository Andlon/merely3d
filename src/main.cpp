#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>
#include <algorithm>

#include <merely3d/window.hpp>

#include <Eigen/Geometry>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Key;
using merely3d::Action;
using merely3d::EventHandler;
using merely3d::Material;
using merely3d::Color;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;
using merely3d::Vector2;

using Eigen::Vector3f;

struct CameraKeyController : public merely3d::EventHandler
{
    struct Strafe
    {
        bool left;
        bool right;
        bool forward;
        bool backward;
        bool up;
        bool down;

        Strafe() : left(false), right(false), forward(false), backward(false), up(false), down(false) {}
    };

    Strafe _strafe;

    virtual bool key_press(Window & window,
                           Key key,
                           Action action,
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

        const auto strafe_enabled = action == Action::Press || action == Action::Repeat;

        switch (key)
        {
            case Key::W: _strafe.forward = strafe_enabled; break;
            case Key::S: _strafe.backward = strafe_enabled; break;
            case Key::A: _strafe.left = strafe_enabled; break;
            case Key::D: _strafe.right = strafe_enabled; break;
            case Key::Space: _strafe.up = strafe_enabled; break;
            case Key::C: _strafe.down = strafe_enabled; break;
        }

        if (action == Action::Press || action == Action::Repeat)
        {
            switch (key)
            {
                case Key::Left: rotation_axis += z; break;
                case Key::Right: rotation_axis -= z; break;
                case Key::Up: rotation_axis += camera.right(); break;
                case Key::Down: rotation_axis -= camera.right(); break;
            }

            rotation_axis.normalize();

            const AngleAxisf rotation = AngleAxisf(ANGLE_STEP, rotation_axis);
            const Eigen::Quaternionf new_orientation = rotation * camera.orientation();

            camera.set_orientation(new_orientation);
        }
    }

    virtual void before_frame(Window & window, double time_since_prev)
    {
        using Eigen::AngleAxisf;

        const auto STRAFE_VELOCITY = 0.5;
        const auto dt = std::max(0.25, time_since_prev);
        auto & camera = window.camera();

        Eigen::Vector3f strafe_direction = Eigen::Vector3f::Zero();
        const auto x = Eigen::Vector3f(1.0, 0.0, 0.0);
        const auto y = Eigen::Vector3f(0.0, 1.0, 0.0);
        const auto z = Eigen::Vector3f(0.0, 0.0, 1.0);

        Eigen::Vector3f rotation_axis = Eigen::Vector3f::Zero();

        if (_strafe.left) strafe_direction -= camera.right();
        if (_strafe.right) strafe_direction += camera.right();
        if (_strafe.forward) strafe_direction += camera.direction();
        if (_strafe.backward) strafe_direction -= camera.direction();
        if (_strafe.up) strafe_direction += z;
        if (_strafe.down) strafe_direction -= z;

        strafe_direction.normalize();

        camera.set_position(camera.position() + dt * STRAFE_VELOCITY * strafe_direction);
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

        window.add_event_handler(std::shared_ptr<EventHandler>(new CameraKeyController()));

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
