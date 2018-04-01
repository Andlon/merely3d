#include <merely3d/frame.hpp>

using Eigen::Transform;
using Eigen::Translation3f;
using Eigen::DiagonalMatrix;

namespace merely3d
{
    void Frame::clear()
    {
        _rectangles.clear();
    }

    // void Frame::draw_box(const Vector3 & half_extents,
    //                      const Position & position,
    //                      const Orientation & orientation,
    //                      const Material & material)
    // {
    //     Box box;
    //     box.half_extents = half_extents;
    //     box.transform = Eigen::Translation(position) * orientation
    //     box.position = position;
    //     box.orientation = orientation;
    //     box.material = material;
    //     _boxes.push_back(box);
    // }

    // void Frame::draw_triangle(const Position &a, const Position & b, const Position & c,
    //                           const Position & position, const Orientation & orientation,
    //                           const Material & material)
    // {
    //     Triangle triangle;
    //     triangle.a = a;
    //     triangle.b = b;
    //     triangle.c = c;
    //     triangle.position = position;
    //     triangle.orientation = orientation;
    //     triangle.material = material;
    //     _triangles.push_back(triangle);
    // }

    void Frame::draw_rectangle(const Vector2 & half_extents,
                        const Position & position,
                        const Orientation & orientation,
                        const Material & material)
    {
        const auto scaling = Eigen::DiagonalMatrix<float, 3>(half_extents.x(), half_extents.y(), 0.0);
        Primitive rectangle;
        rectangle.transform = Eigen::Translation3f(position) *  orientation * scaling;
        rectangle.material = material;
        _rectangles.push_back(rectangle);
    }
}