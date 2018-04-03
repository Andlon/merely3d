#pragma once

#include <merely3d/types.hpp>
#include <merely3d/material.hpp>
#include <merely3d/primitives.hpp>

// TODO: Use pimpl to avoid these includes, see below
#include <vector>

namespace merely3d
{
    class CommandBuffer;

    class Frame
    {
    public:
        Frame(const Frame &) = delete;
        Frame(Frame &&) = delete;

        void draw_rectangle(const Rectangle & shape,
                            const Position & position = Position::Zero(),
                            const Orientation & orientation = Orientation::Identity(),
                            const Material & material = Material());

        void draw_box(const Box & shape,
                      const Position & position = Position::Zero(),
                      const Orientation & orientation = Orientation::Identity(),
                      const Material & material = Material());

    private:
        Frame(CommandBuffer * buffer) : _buffer(buffer) {}
        ~Frame() {}
        friend class Window;

        CommandBuffer * _buffer;
    };
}