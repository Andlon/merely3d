#pragma once

#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>

#include <Eigen/Dense>
#include <vector>

namespace merely3d
{
    template <typename Shape>
    struct Renderable
    {
        Eigen::Vector3f     position;
        Eigen::Quaternionf  orientation;
        Material            material;
        Shape               shape;
    };

    class CommandBuffer
    {
    public:
        void clear();
        void push_rectangle(const Rectangle & rectangle,
                            const Eigen::Vector3f & position,
                            const Eigen::Quaternionf & orientation,
                            const Material & material);

        const std::vector<Renderable<Rectangle>> & rectangles() const;

    private:
        std::vector<Renderable<Rectangle>> _rectangles;
    };

    inline void CommandBuffer::clear()
    {
        _rectangles.clear();
    }

    inline void CommandBuffer::push_rectangle(const Rectangle & rectangle,
                                              const Eigen::Vector3f & position,
                                              const Eigen::Quaternionf & orientation,
                                              const Material & material)
    {
        Renderable<Rectangle> renderable;
        renderable.shape = rectangle;
        renderable.position = position;
        renderable.orientation = orientation;
        renderable.material = material;
        _rectangles.push_back(renderable);
    }

    inline const std::vector<Renderable<Rectangle>> & CommandBuffer::rectangles() const
    {
        return _rectangles;
    }
}

