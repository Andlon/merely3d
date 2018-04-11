#pragma once

#include <Eigen/Dense>

#include <type_traits>

#include <merely3d/types.hpp>
#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>
#include <merely3d/renderable.hpp>

namespace merely3d
{
    class CommandBuffer;

    class Frame
    {
    public:
        Frame(const Frame &) = delete;
        Frame(Frame &&) = delete;

        template <typename Shape>
        void draw(const Renderable<Shape> &renderable);

    private:
        Frame(CommandBuffer * buffer) : _buffer(buffer) {}
        ~Frame() {}
        friend class Window;

        CommandBuffer * _buffer;
    };

    template <>
    void Frame::draw(const merely3d::Renderable<Box> & renderable);

    template <>
    void Frame::draw(const merely3d::Renderable<Rectangle> & rectangle);

    template <>
    void Frame::draw(const merely3d::Renderable<Sphere> & sphere);

    template <typename Shape>
    void Frame::draw(const merely3d::Renderable<Shape> &renderable)
    {
        static_assert(!std::is_same<Shape, Shape>::value, "");
    }
}