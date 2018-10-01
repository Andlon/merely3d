#pragma once

#include <Eigen/Dense>

#include <type_traits>

#include <merely3d/types.hpp>
#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>
#include <merely3d/renderable.hpp>
#include <merely3d/mesh.hpp>

namespace merely3d
{
    class CommandBuffer;

    class Frame
    {
    public:
        Frame(const Frame &) = delete;

        template <typename Shape>
        void draw(const Renderable<Shape> &renderable);

        void draw_line(const Line & line);

        void draw_particle(const Particle & particle);

        /// Returns the number of seconds since the beginning of the previous frame.
        double time_since_prev_frame() const;

    private:
        Frame(CommandBuffer * buffer, double delta_elapsed_time)
            : _buffer(buffer), _delta_time(delta_elapsed_time) {}
        Frame(Frame && frame) : _buffer(frame._buffer), _delta_time(frame._delta_time) {}
        ~Frame() {}
        friend class Window;

        CommandBuffer * _buffer;
        double _delta_time;
    };

    template <>
    void Frame::draw(const merely3d::Renderable<Box> & renderable);

    template <>
    void Frame::draw(const merely3d::Renderable<Rectangle> & rectangle);

    template <>
    void Frame::draw(const merely3d::Renderable<Sphere> & sphere);

    template <>
    void Frame::draw(const merely3d::Renderable<StaticMesh> & mesh);

    template <typename Shape>
    void Frame::draw(const merely3d::Renderable<Shape> &renderable)
    {
        static_assert(!std::is_same<Shape, Shape>::value, "");
    }
}
