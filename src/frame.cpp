#include <merely3d/frame.hpp>

namespace merely3d
{
    void Frame::clear()
    {
        _boxes.clear();
    }

    void Frame::draw_box(const Vector3 & half_extents,
                         const Position & position,
                         const Orientation & orientation,
                         const Material & material)
    {
        Box box;
        box.half_extents = half_extents;
        box.position = position;
        box.orientation = orientation;
        box.material = material;
        _boxes.push_back(std::move(box));
    }
}