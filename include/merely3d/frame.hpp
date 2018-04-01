#pragma once

#include <merely3d/types.hpp>
#include <merely3d/material.hpp>

// TODO: Use pimpl to avoid these includes, see below
#include <vector>

namespace merely3d
{
    class Frame
    {
    public:
        Frame(const Frame &) = delete;
        Frame(Frame &&) = delete;

        // void draw_box(const Vector3 & half_extents,
        //               const Position & position,
        //               const Orientation & orientation,
        //               const Material & material = Material());

        // void draw_triangle(const Position &a, const Position & b, const Position & c,
        //                    const Position & position, const Orientation & orientation,
        //                    const Material & material = Material());

        void draw_rectangle(const Vector2 & half_extents,
                            const Position & position,
                            const Orientation & orientation,
                            const Material & material = Material());

    private:
        void clear();

        Frame() {}
        ~Frame() {}
        friend class Window;

        // TODO: Use impl to hide Frame implementation details

        struct Primitive
        {
            Eigen::Affine3f transform;
            Material        material;
        };

        std::vector<Primitive> _rectangles;
    };
}