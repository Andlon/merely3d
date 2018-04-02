#pragma once

#include <merely3d/material.hpp>
#include <Eigen/Dense>

namespace merely3d
{
    struct Rectangle
    {
        Rectangle() : extents(Eigen::Vector2f::Zero()) {}
        Rectangle(const Eigen::Vector2f & extents) : extents(extents) {}
        Rectangle(float x_extent, float y_extent)
            : extents(Eigen::Vector2f(x_extent, y_extent)) {}

        Eigen::Vector2f extents;
    };
}