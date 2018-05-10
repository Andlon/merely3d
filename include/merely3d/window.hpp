#pragma once

#include <memory>

#include <merely3d/frame.hpp>
#include <merely3d/camera.hpp>
#include <merely3d/events.hpp>

struct GLFWwindow;

namespace merely3d
{
    class CommandBuffer;

    /// The cursor mode determines how the window handles cursor movement.
    enum class CursorMode
    {
        /// Normal cursor movement.
        Normal,
        /// Normal cursor movement, but the cursor itself is invisible.
        Hidden,
        /// Cursor is disabled, but motion is still registered. Often used to implement
        /// mouse look cameras.
        Disabled
    };

    struct WindowSize
    {
        int width;
        int height;
    };

    struct ScreenCoords
    {
        double x;
        double y;

        ScreenCoords() : x(0.0), y(0.0) {}
        ScreenCoords(double x, double y) : x(x), y(y) {}
    };

    class Window final
    {
    public:
        Window(Window && other);

        ~Window();

        bool should_close() const;

        template <typename RenderFunc>
        void render_frame(RenderFunc && render_func)
        {
            make_current();
            poll_events();
            auto frame = begin_frame();
            std::forward<RenderFunc>(render_func)(frame);
            render_frame_impl(frame);
        }

        Camera & camera();
        const Camera & camera() const;

        void add_event_handler(std::shared_ptr<EventHandler> handler);

        /// Returns a pointer to the underlying GLFW window.
        ///
        /// Using this pointer is very unsafe, and in principle there are no
        /// guarantees that changing any GLFW state can be done safely.
        /// Use with caution! (And preferably, don't use at all).
        GLFWwindow * glfw_window();

        void make_current();

        void poll_events();

        void set_cursor_mode(CursorMode mode);

        /// Returns the size of the windows in screen coordinates (not necessarily pixels).
        WindowSize size() const;

        Action get_last_key_action(Key key) const;

        ScreenCoords get_current_cursor_position() const;

        /// Compute the point on the near plane of the view frustrum
        /// corresponding to the given screen coordinates.
        ///
        /// The coordinates returned are given in view space,
        /// so in order to obtain world coordinates, the inverse view transform
        /// (as provided by the Camera) must be used.
        Eigen::Vector3f unproject_screen_coordinates(const ScreenCoords & coords);

        /// Returns the (vertical) field of view used for perspective transformation.
        float fovy() const;

        /// Set the vertical field of view used for perspective transformation.
        ///
        /// The horizontal FOV is determined by the vertical FOV and the aspect
        /// ratio of the viewport. Must be a positive number in the interval (0, PI).
        void set_fovy(float fovy);

    private:
        friend class WindowBuilder;
        friend void dispatch_key_event(Window *, Key, Action, int, int);
        friend void dispatch_mouse_button_event(Window *, MouseButton, Action, int);
        friend void dispatch_mouse_move_event(Window * window, double xpos, double ypos);
        friend void dispatch_character_input_event(Window * window, unsigned int codepoint);
        friend void dispatch_scroll_event(Window * window, double xoffset, double yoffset);
        friend void dispatch_mouse_enter_event(Window * window, bool entered);
        class WindowData;

        Window(WindowData * data);

        Frame begin_frame();
        void end_frame();
        void render_frame_impl(Frame & frame);
        CommandBuffer * get_command_buffer();

        WindowData * _d;
    };

    class WindowBuilder
    {
    public:
        WindowBuilder()
            :   _width(640),
                _height(480),
                _samples(0)
        {

        }

        WindowBuilder dimensions(unsigned int width, unsigned int height) const
        {
            auto result = *this;
            result._width = width;
            result._height = height;
            return result;
        }

        WindowBuilder title(std::string title) const
        {
            auto result = *this;
            result._title = std::move(title);
            return result;
        }

        WindowBuilder multisampling(unsigned int samples) const
        {
            auto result = *this;
            result._samples = samples;
            return result;
        }

        Window build() const;

    private:
        int             _width;
        int             _height;
        unsigned int    _samples;
        std::string     _title;
    };
}
