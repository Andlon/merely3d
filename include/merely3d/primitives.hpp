#pragma once

#include <merely3d/types.hpp>
#include <merely3d/color.hpp>
#include <Eigen/Dense>

namespace merely3d
{
    struct Rectangle
    {
        Rectangle() : extents(Eigen::Vector2f::Zero()) {}
        explicit Rectangle(const Eigen::Vector2f & extents) : extents(extents) {}
        Rectangle(float x_extent, float y_extent)
            : extents(Eigen::Vector2f(x_extent, y_extent)) {}

        UnalignedVector2f extents;
    };

    struct Box
    {
        Box() : extents(Eigen::Vector3f::Zero()) {}
        explicit Box(const Eigen::Vector3f & extents) : extents(extents) {}
        Box(float x_extent, float y_extent, float z_extent)
            : extents(Eigen::Vector3f(x_extent, y_extent, z_extent)) {}

        Eigen::Vector3f extents;
    };

    struct Sphere
    {
        Sphere() : radius(1.0) {}
        explicit Sphere(float radius) : radius(radius) {}

        float radius;
    };

    struct Line
    {
        Line(const Eigen::Vector3f & from, const Eigen::Vector3f & to, const Color & color = blue())
                : from(from), to(to), color(color) {}

        Eigen::Vector3f from;
        Eigen::Vector3f to;
        Color           color;
    };
}