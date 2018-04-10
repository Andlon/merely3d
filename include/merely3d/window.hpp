#pragma once

#include <memory>

#include <merely3d/frame.hpp>
#include <merely3d/camera.hpp>
#include <merely3d/events.hpp>

class GLFWwindow;

namespace merely3d
{
    class CommandBuffer;

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
            begin_frame();
            auto buffer = get_command_buffer();
            Frame frame(buffer);
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

    private:
        friend class WindowBuilder;
        friend void dispatch_key_event(Window *, Key, Action, int, int);
        class WindowData;

        Window(WindowData * data);

        void begin_frame();
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
                _height(480)
        {

        }

        WindowBuilder dimensions(unsigned int width, unsigned int height)
        {
            auto result = *this;
            result._width = width;
            result._height = height;
            return result;
        }

        WindowBuilder title(std::string title)
        {
            auto result = *this;
            result._title = std::move(title);
            return result;
        }

        Window build() const;

    private:
        int         _width;
        int         _height;
        std::string _title;
    };
}
