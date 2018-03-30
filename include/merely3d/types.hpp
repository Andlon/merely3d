#pragma once

// TODO: More specific includes?
#include <Eigen/Dense>

namespace merely3d
{
    typedef Eigen::Matrix<float, 3, 1> Position;
    typedef Eigen::Quaternion<float>   Orientation;
    typedef Eigen::Matrix<float, 3, 1> Vector3;
    typedef Eigen::Matrix<float, 2, 1> Vector2;
}