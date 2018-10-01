#include <merely3d/frame.hpp>

#include "command_buffer.hpp"

using Eigen::Transform;
using Eigen::Translation3f;
using Eigen::DiagonalMatrix;

namespace merely3d
{
    double Frame::time_since_prev_frame() const
    {
        return _delta_time;
    }

    template <>
    void Frame::draw(const merely3d::Renderable<Box> &renderable)
    {
        _buffer->push_renderable(renderable);
    }

    template <>
    void Frame::draw(const merely3d::Renderable<Rectangle> &renderable)
    {
        _buffer->push_renderable(renderable);
    }

    template <>
    void Frame::draw(const merely3d::Renderable<Sphere> & sphere)
    {
        _buffer->push_renderable(sphere);
    }

    template <>
    void Frame::draw(const merely3d::Renderable<StaticMesh> & mesh)
    {
        _buffer->push_renderable(mesh);
    }

    void Frame::draw_line(const merely3d::Line &line)
    {
        _buffer->push_line(line);
    }

    void Frame::draw_particle(const merely3d::Particle & particle)
    {
        _buffer->push_particle(particle);
    }
}
