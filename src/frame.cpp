#include <merely3d/frame.hpp>

#include "command_buffer.hpp"

using Eigen::Transform;
using Eigen::Translation3f;
using Eigen::DiagonalMatrix;

namespace merely3d
{
    void Frame::draw_rectangle(const Rectangle & shape,
                        const Position & position,
                        const Orientation & orientation,
                        const Material & material)
    {
        _buffer->push_rectangle(shape, position, orientation, material);
    }
}