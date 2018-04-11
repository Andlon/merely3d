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

        void draw_rectangle(const Rectangle & shape,
                            const Eigen::Vector3f & position = Eigen::Vector3f::Zero(),
                            const Eigen::Quaternionf & orientation = Eigen::Quaternionf::Identity(),
                            const Material & material = Material());

        void draw_box(const Box & shape,
                      const Eigen::Vector3f & position = Eigen::Vector3f::Zero(),
                      const Eigen::Quaternionf & orientation = Eigen::Quaternionf::Identity(),
                      const Material & material = Material());

        template <typename Shape>
        void draw_renderable(const Renderable<Shape> & renderable);

    private:
        Frame(CommandBuffer * buffer) : _buffer(buffer) {}
        ~Frame() {}
        friend class Window;

        CommandBuffer * _buffer;
    };

    template <>
    void Frame::draw_renderable(const merely3d::Renderable<Box> & renderable);

    template <>
    void Frame::draw_renderable(const merely3d::Renderable<Rectangle> & rectangle);

    template <typename Shape>
    void Frame::draw_renderable(const merely3d::Renderable<Shape> & renderable)
    {
        static_assert(!std::is_same<Shape, Shape>::value, "");
    }

    inline void Frame::draw_box(const Box &shape,
                         const Eigen::Vector3f &position,
                         const Eigen::Quaternionf &orientation,
                         const Material &material)
    {
        draw_renderable(Renderable<Box>(shape, position, orientation, Eigen::Vector3f(1.0f, 1.0f, 1.0f), material));
    }

    inline void Frame::draw_rectangle(const Rectangle & shape,
                               const Eigen::Vector3f & position,
                               const Eigen::Quaternionf & orientation,
                               const Material & material)
    {
        draw_renderable(Renderable<Rectangle>(shape, position, orientation, Eigen::Vector3f(1.0f, 1.0f, 1.0f), material));
    }
}