#pragma once

#include <memory>

#include <merely3d/frame.hpp>
#include <merely3d/camera.hpp>

namespace merely3d
{
    class Window final
    {
    public:
        Window(Window && other);

        ~Window();

        bool should_close() const;

        template <typename RenderFunc>
        void render_frame(RenderFunc && render_func)
        {
            // TODO: Re-use last frame to be able to reuse buffers
            Frame frame;
            std::forward<RenderFunc>(render_func)(frame);
            render_frame_impl(frame);
        }

        Camera & camera();
        const Camera & camera() const;

    private:
        friend class WindowBuilder;
        class WindowData;

        Window(WindowData * data);

        void render_frame_impl(Frame & frame);

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