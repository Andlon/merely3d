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
        Shape               shape;
        Eigen::Vector3f     position;
        Eigen::Quaternionf  orientation;
        Material            material;

        Renderable(const Shape & shape,
                   const Eigen::Vector3f & position,
                   const Eigen::Quaternionf & orientation,
                   const Material & material)
            :   shape(shape),
                position(position),
                orientation(orientation),
                material(material)
        {}
    };

    class CommandBuffer
    {
    public:
        void clear();
        void push_rectangle(const Rectangle & rectangle,
                            const Eigen::Vector3f & position,
                            const Eigen::Quaternionf & orientation,
                            const Material & material);

        void push_box(const Box & box,
                      const Eigen::Vector3f & position,
                      const Eigen::Quaternionf & orientation,
                      const Material & material);

        const std::vector<Renderable<Rectangle>> &  rectangles() const;
        const std::vector<Renderable<Box>> &        boxes() const;

    private:
        std::vector<Renderable<Rectangle>>  _rectangles;
        std::vector<Renderable<Box>>        _boxes;
    };

    inline void CommandBuffer::clear()
    {
        _rectangles.clear();
        _boxes.clear();
    }

    inline void CommandBuffer::push_rectangle(const Rectangle & rectangle,
                                              const Eigen::Vector3f & position,
                                              const Eigen::Quaternionf & orientation,
                                              const Material & material)
    {
        _rectangles.push_back(Renderable<Rectangle>(rectangle, position, orientation, material));
    }

    inline void CommandBuffer::push_box(const Box & box,
                                        const Eigen::Vector3f & position,
                                        const Eigen::Quaternionf & orientation,
                                        const Material & material)
    {
        _boxes.push_back(Renderable<Box>(box, position, orientation, material));
    }

    inline const std::vector<Renderable<Rectangle>> & CommandBuffer::rectangles() const
    {
        return _rectangles;
    }

    inline const std::vector<Renderable<Box>> & CommandBuffer::boxes() const
    {
        return _boxes;
    }
}

