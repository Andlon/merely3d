#pragma once

#include "types.hpp"
#include "material.hpp"

// TODO: Use pimpl to avoid these includes, see below
#include <vector>

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
        void clear();

        Frame() {}
        ~Frame() {}
        friend class Window;

        // TODO: Use impl to hide Frame implementation details

        struct Box
        {
            Vector3     half_extents;
            Position    position;
            Orientation orientation;
            Material    material;
        };

        std::vector<Box> _boxes;
    };
}