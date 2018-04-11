#pragma once

#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>
#include <merely3d/renderable.hpp>

#include <Eigen/Dense>

#include <type_traits>
#include <vector>

namespace merely3d
{
    class CommandBuffer
    {
    public:
        void clear();

        template <typename Shape>
        void push_renderable(const Renderable<Shape> & renderable);

        const std::vector<Renderable<Rectangle>> &  rectangles() const;
        const std::vector<Renderable<Box>> &        boxes() const;
        const std::vector<Renderable<Sphere>> &     spheres() const;

    private:
        std::vector<Renderable<Rectangle>>  _rectangles;
        std::vector<Renderable<Box>>        _boxes;
        std::vector<Renderable<Sphere>>     _spheres;
    };

    inline void CommandBuffer::clear()
    {
        _rectangles.clear();
        _boxes.clear();
        _spheres.clear();
    }

    inline const std::vector<Renderable<Rectangle>> & CommandBuffer::rectangles() const
    {
        return _rectangles;
    }

    inline const std::vector<Renderable<Box>> & CommandBuffer::boxes() const
    {
        return _boxes;
    }

    inline const std::vector<Renderable<Sphere>> & CommandBuffer::spheres() const
    {
        return _spheres;
    }

    template<typename Shape>
    inline void CommandBuffer::push_renderable(const Renderable <Shape> & renderable)
    {
        static_assert(!std::is_same<Shape, Shape>::value, "Shape not supported");
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Box> & renderable)
    {
        _boxes.push_back(renderable);
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Rectangle> & renderable)
    {
        _rectangles.push_back(renderable);
    }

    template <>
    inline void CommandBuffer::push_renderable(const Renderable<Sphere> &renderable)
    {
        _spheres.push_back(renderable);
    }
}

