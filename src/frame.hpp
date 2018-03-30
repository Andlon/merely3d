#pragma once

#include "types.hpp"
#include "material.hpp"

namespace merely3d
{
    class Frame
    {
    public:
        Frame(const Frame &) = delete;
        Frame(Frame &&) = delete;

        void draw_box(const Vector3 & half_extents,
                      const Position & position,
                      const Orientation & orientation,
                      const Material & material = Material());

    private:
        Frame() {}
        ~Frame() {}
        friend class Window;
    };
}