#pragma once

#include <memory>

#include "frame.hpp"

// Forward declare GLFWwindow
struct GLFWwindow;

namespace merely3d
{
    class Window
    {
    public:

        // TODO: Can we use a default move constructor here?
        Window(Window && other)
            : _glfw_window(std::move(other._glfw_window))
        {

        }

        ~Window()
        {

        }

        bool should_close() const;

        template <typename RenderFunc>
        void render_frame(RenderFunc && render_func)
        {
            // TODO: Re-use last frame to be able to reuse buffers
            Frame frame;
            std::forward<RenderFunc>(render_func)(frame);
            render_frame_impl(frame);
        }

    private:
        void render_frame_impl(Frame & frame);

        typedef void(*GlfwWindowDestroyFunc)(GLFWwindow *);
        typedef std::unique_ptr<GLFWwindow, GlfwWindowDestroyFunc> GlfwWindowPtr;

        Window(GlfwWindowPtr glfw_window)
            : _glfw_window(std::move(glfw_window)),
              _viewport_size(0, 0)
        {

        }

        friend class WindowBuilder;

        GlfwWindowPtr _glfw_window;

        std::pair<int, int> _viewport_size;
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