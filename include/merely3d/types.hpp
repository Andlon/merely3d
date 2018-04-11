#pragma once

#include <Eigen/Dense>

namespace merely3d
{
    // Unaligned types are needed to ensure safe usage when storing types
    // in containers or in structs.
    typedef Eigen::Matrix<float, 2, 1, Eigen::DontAlign> UnalignedVector2f;
    typedef Eigen::Quaternion<float, Eigen::DontAlign>   UnalignedQuaternionf;
}