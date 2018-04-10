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

struct CameraLookController : public merely3d::EventHandler
{
    struct Look
    {
        bool up;
        bool down;
        bool left;
        bool right;

        Look() : up(false), down(false), left(false), right(false) {}
    };

    Look _look;

    virtual bool key_press(Window & window,
                           Key key,
                           Action action,
                           int scancode,
                           int modifiers) override
    {
        const auto look_enabled = action == Action::Press || action == Action::Repeat;

        switch (key)
        {
            case Key::Left: _look.left = look_enabled; return true;
            case Key::Right: _look.right = look_enabled; return true;
            case Key::Up: _look.up = look_enabled; return true;
            case Key::Down: _look.down = look_enabled; return true;
            default: return false;
        }
    }

    virtual void before_frame(Window & window, double time_since_prev)
    {
        using Eigen::Vector3f;
        using Eigen::AngleAxisf;
        using Eigen::Quaternionf;

        auto & camera = window.camera();

        const auto dt = std::max(0.25, time_since_prev);

        const auto ANGULAR_VELOCITY = 1.0;
        const auto rot_angle = ANGULAR_VELOCITY * time_since_prev;

        float horizontal_rot = 0.0;
        float vertical_rot = 0.0;

        const Vector3f vertical_axis = camera.right();
        const Vector3f horizontal_axis = Vector3f::UnitZ();

        if (_look.up) vertical_rot += rot_angle;
        if (_look.down) vertical_rot -= rot_angle;
        if (_look.left) horizontal_rot += rot_angle;
        if (_look.right) horizontal_rot -= rot_angle;

        const auto vrot = AngleAxisf(vertical_rot, vertical_axis);
        const auto hrot = AngleAxisf(horizontal_rot, horizontal_axis);
        const Quaternionf new_orientation = hrot * vrot * camera.orientation();
        camera.set_orientation(new_orientation);
    }

};

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

        auto & camera = window.camera();
        const auto strafe_enabled = action == Action::Press || action == Action::Repeat;

        switch (key)
        {
            case Key::W: _strafe.forward = strafe_enabled; return true;
            case Key::S: _strafe.backward = strafe_enabled; return true;
            case Key::A: _strafe.left = strafe_enabled; return true;
            case Key::D: _strafe.right = strafe_enabled; return true;
            case Key::Space: _strafe.up = strafe_enabled; return true;
            case Key::C: _strafe.down = strafe_enabled; return true;
            default: return false;
        }
    }

    virtual void before_frame(Window & window, double time_since_prev)
    {
        using Eigen::AngleAxisf;
        using Eigen::Vector3f;

        const auto STRAFE_VELOCITY = 0.5;
        const auto dt = std::max(0.25, time_since_prev);
        auto & camera = window.camera();

        Vector3f strafe_direction = Eigen::Vector3f::Zero();

        if (_strafe.left) strafe_direction -= camera.right();
        if (_strafe.right) strafe_direction += camera.right();
        if (_strafe.forward) strafe_direction += camera.direction();
        if (_strafe.backward) strafe_direction -= camera.direction();
        if (_strafe.up) strafe_direction += Vector3f::UnitZ();
        if (_strafe.down) strafe_direction -= Vector3f::UnitZ();

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

        window.add_event_handler(std::shared_ptr<EventHandler>(new CameraLookController()));
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
