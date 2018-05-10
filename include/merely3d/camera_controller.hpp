#pragma once

#include <merely3d/window.hpp>

namespace merely3d
{
    /// A camera controller that allows controlling orientation with mouse movements.
    ///
    /// Note that for the time being, it is only active whenever the left mouse button
    /// is held down. This is an artificial restriction due to the fact that, at present,
    /// this functionality is the only thing that is needed for our target application
    /// (the game physics class at RWTH Aachen).
    class CameraLookMouseController : public merely3d::EventHandler
    {
    public:
        CameraLookMouseController()
                : xpos(0.0), ypos(0.0), xsens(1.0), ysens(1.0), enabled(false)
        { }

        float horizontal_look_sensitivity() const
        {
            return xsens;
        }

        void set_horizontal_look_sensitivity(float sens)
        {
            xsens = sens;
        }

        float vertical_look_sensitivity() const
        {
            return ysens;
        }

        void set_vertical_look_sensitivity(float sens)
        {
            ysens = sens;
        }

        bool mouse_button_press(Window &window,
                                MouseButton button,
                                Action action,
                                int) override
        {
            using merely3d::MouseButton;

            if (button == MouseButton::Left)
            {
                const auto should_enable = action == Action::Press || action == Action::Repeat;
                enable(window, should_enable);
                return true;
            }

            return false;
        }

        bool mouse_move(Window &, double xpos, double ypos) override
        {
            bool accept_event = false;

            if (this->enabled)
            {
                this->xdelta += xpos - this->xpos;
                this->ydelta += ypos - this->ypos;
                accept_event = true;
            }

            this->xpos = xpos;
            this->ypos = ypos;

            return accept_event;
        }

        void mouse_leave(Window & window) override
        {
            enable(window, false);
        }

        void before_frame(Window &window, Frame &) override
        {
            using Eigen::Quaternionf;
            using Eigen::AngleAxisf;
            using Eigen::Vector3f;

            auto & camera = window.camera();

            if (this->enabled)
            {
                const auto window_size = window.size();
                const auto aspect_ratio = window_size.width > 0 && window_size.height > 0
                                          ? static_cast<double>(window_size.width) / static_cast<double>(window_size.height)
                                          : 1.0;

                const auto fovy = window.fovy();
                const auto fovx = 2.0 * atan(tan(fovy / 2.0) * aspect_ratio);

                float horizontal_rot = - xsens * static_cast<float>(xdelta * fovx / window_size.width);
                float vertical_rot = - ysens * static_cast<float>(ydelta * fovy / window_size.height);

                const Vector3f vertical_axis = camera.right();
                const Vector3f horizontal_axis = Vector3f::UnitZ();

                const auto vrot = AngleAxisf(vertical_rot, vertical_axis);
                const auto hrot = AngleAxisf(horizontal_rot, horizontal_axis);
                const Quaternionf new_orientation = hrot * vrot * camera.orientation();
                camera.set_orientation(new_orientation);
            }

            this->xdelta = 0.0;
            this->ydelta = 0.0;
        }

    private:
        void enable(Window & window, bool enable)
        {
            this->enabled = enable;

            if (enable)
            {
                window.set_cursor_mode(merely3d::CursorMode::Disabled);
            }
            else
            {
                xdelta = 0.0;
                ydelta = 0.0;
                window.set_cursor_mode(merely3d::CursorMode::Normal);
            }
        }

        // Recorded mouse positions
        double xpos;
        double ypos;
        double xdelta;
        double ydelta;
        float xsens;
        float ysens;
        bool enabled;
    };

    /// A camera controller that allows controlling orientation with the arrow keys.
    class CameraLookKeyController : public merely3d::EventHandler
    {
    public:
        CameraLookKeyController()
                : _angular_velocity(1.0f)
        {

        }

        float angular_look_velocity() const
        {
            return _angular_velocity;
        }

        void set_angular_look_velocity(float velocity)
        {
            _angular_velocity = velocity;
        }

        virtual bool key_press(Window &,
                               Key key,
                               Action action,
                               int,
                               int) override
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

        virtual void before_frame(Window & window, Frame & frame) override
        {
            using Eigen::Vector3f;
            using Eigen::AngleAxisf;
            using Eigen::Quaternionf;

            auto & camera = window.camera();

            const auto dt = std::min(0.25, frame.time_since_prev_frame());
            const auto rot_angle = _angular_velocity * dt;

            float horizontal_rot = 0.0f;
            float vertical_rot = 0.0f;

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

    private:
        struct Look
        {
            bool up;
            bool down;
            bool left;
            bool right;

            Look() : up(false), down(false), left(false), right(false) {}
        };

        Look    _look;
        float   _angular_velocity;
    };

    /// A camera controller that allows controlling camera translational movement
    /// with the WASD (forward-left-back-right) and RC (up-down) keys.
    class CameraStrafeKeyController : public merely3d::EventHandler
    {
    public:

        CameraStrafeKeyController()
                : _velocity(4.0f)
        {}

        float strafe_velocity() const
        {
            return _velocity;
        }

        void set_strafe_velocity(float velocity)
        {
            _velocity = velocity;
        }

        virtual bool key_press(Window &,
                               Key key,
                               Action action,
                               int,
                               int) override
        {
            using Eigen::AngleAxisf;

            const auto strafe_enabled = action == Action::Press || action == Action::Repeat;

            switch (key)
            {
                case Key::W: _strafe.forward = strafe_enabled; return true;
                case Key::S: _strafe.backward = strafe_enabled; return true;
                case Key::A: _strafe.left = strafe_enabled; return true;
                case Key::D: _strafe.right = strafe_enabled; return true;
                case Key::R: _strafe.up = strafe_enabled; return true;
                case Key::C: _strafe.down = strafe_enabled; return true;
                default: return false;
            }
        }

        virtual void before_frame(Window & window, Frame & frame) override
        {
            using Eigen::AngleAxisf;
            using Eigen::Vector3f;

            const auto dt = std::min(0.25, frame.time_since_prev_frame());
            auto & camera = window.camera();

            Vector3f strafe_direction = Eigen::Vector3f::Zero();

            if (_strafe.left) strafe_direction -= camera.right();
            if (_strafe.right) strafe_direction += camera.right();
            if (_strafe.forward) strafe_direction += camera.direction();
            if (_strafe.backward) strafe_direction -= camera.direction();
            if (_strafe.up) strafe_direction += Vector3f::UnitZ();
            if (_strafe.down) strafe_direction -= Vector3f::UnitZ();

            strafe_direction.normalize();

            camera.set_position(camera.position() + dt * _velocity * strafe_direction);
        }

    private:
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
        float  _velocity;
    };

    /// A camera controller that allows manipulating translational
    /// and rotational motion of the camera.
    ///
    /// This is simply a convenience class which combines
    /// CameraStrafeKeyController, CameraLookKeyController and
    /// CameraLookMouseController into a single controller.
    class CameraController : public merely3d::EventHandler
    {
    public:

        float strafe_velocity() const
        {
            return _strafe.strafe_velocity();
        }

        void set_strafe_velocity(float velocity)
        {
            _strafe.set_strafe_velocity(velocity);
        }

        float horizontal_look_sensitivity() const
        {
            return _look_mouse.horizontal_look_sensitivity();
        }

        void set_horizontal_look_sensitivity(float sens)
        {
            _look_mouse.set_horizontal_look_sensitivity(sens);
        }

        float vertical_look_sensitivity() const
        {
            return _look_mouse.vertical_look_sensitivity();
        }

        void set_vertical_look_sensitivity(float sens)
        {
            _look_mouse.set_vertical_look_sensitivity(sens);
        }

        float angular_look_velocity() const
        {
            return _look_key.angular_look_velocity();
        }

        bool key_press(Window &window, Key key, Action action, int scancode, int modifiers) override {
            return _strafe.key_press(window, key, action, scancode, modifiers)
                   || _look_key.key_press(window, key, action, scancode, modifiers)
                   || _look_mouse.key_press(window, key, action, scancode, modifiers);
        }

        bool mouse_button_press(Window &window, merely3d::MouseButton button, Action action, int modifiers) override {
            return _look_mouse.mouse_button_press(window, button, action, modifiers)
                   || _look_key.mouse_button_press(window, button, action, modifiers)
                   || _strafe.mouse_button_press(window, button, action, modifiers);
        }

        bool mouse_move(Window &window, double xpos, double ypos) override {
            return _look_mouse.mouse_move(window, xpos, ypos)
                   || _look_key.mouse_move(window, xpos, ypos)
                   || _strafe.mouse_move(window, xpos, ypos);

        }

        void mouse_enter(Window &window) override {
            _look_mouse.mouse_enter(window);
            _look_key.mouse_enter(window);
            _strafe.mouse_enter(window);
        }

        void mouse_leave(Window &window) override {
            _look_mouse.mouse_leave(window);
            _look_key.mouse_leave(window);
            _strafe.mouse_leave(window);
        }

        bool scroll(Window &window, double xoffset, double yoffset) override {
            return _look_mouse.scroll(window, xoffset, yoffset)
                   || _look_key.scroll(window, xoffset, yoffset)
                   || _strafe.scroll(window, xoffset, yoffset);
        }

        bool character_input(Window &, unsigned int) override {
            return false;
        }

        void before_frame(Window &window, Frame & frame) override {
            _look_mouse.before_frame(window, frame);
            _look_key.before_frame(window, frame);
            _strafe.before_frame(window, frame);
        }

        void after_frame(Window &window, double frame_duration) override {
            _look_mouse.after_frame(window, frame_duration);
            _look_key.after_frame(window, frame_duration);
            _strafe.after_frame(window, frame_duration);
        }

    private:
        CameraStrafeKeyController _strafe;
        CameraLookKeyController   _look_key;
        CameraLookMouseController _look_mouse;
    };
}
