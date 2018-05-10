#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

#include <merely3d/types.hpp>

namespace merely3d
{
    class Camera
    {
    public:
        Camera();

        Eigen::Vector3f position() const;
        Eigen::Vector3f direction() const;
        Eigen::Vector3f up() const;
        Eigen::Vector3f right() const;

        Eigen::Quaternionf orientation() const;
        Eigen::Affine3f transform() const;

        void set_position(const Eigen::Vector3f & position);

        template <typename IntoRotation>
        void set_orientation(const IntoRotation & orientation);

        void look_in(const Eigen::Vector3f & direction,
                     const Eigen::Vector3f & up);

    private:
        Eigen::Vector3f      _position;
        UnalignedQuaternionf _orientation;
    };

    inline Camera::Camera()
        : _position(Eigen::Vector3f::Zero()),
          _orientation(Eigen::Quaternionf::Identity())
    {

    }

    inline Eigen::Vector3f Camera::position() const
    {
        return _position;
    }

    inline Eigen::Vector3f Camera::direction() const
    {
        return _orientation * Eigen::Vector3f(0.0, 0.0, -1.0);
    }

    inline Eigen::Vector3f Camera::up() const
    {
        return _orientation * Eigen::Vector3f(0.0, 1.0, 0.0);
    }

    inline Eigen::Vector3f Camera::right() const
    {
        return _orientation * Eigen::Vector3f(1.0, 0.0, 0.0);
    }

    inline Eigen::Quaternionf Camera::orientation() const
    {
        return _orientation;
    }

    inline Eigen::Affine3f Camera::transform() const
    {
        return Eigen::Translation3f(_position) * _orientation;
    }

    inline void Camera::set_position(const Eigen::Vector3f & position)
    {
        _position = position;
    }

    template <typename IntoRotation>
    inline void Camera::set_orientation(const IntoRotation & orientation)
    {
        _orientation = orientation;
    }

    inline void Camera::look_in(const Eigen::Vector3f & direction,
                         const Eigen::Vector3f & up)
    {
        using Eigen::Vector3f;
        using Eigen::Matrix3f;

        const Vector3f d = direction.normalized();
        Vector3f u = up - up.dot(d) * d;

        if (u.isZero(0.0))
        {
            u = Vector3f(0.0, 1.0, 0.0);
        }
        else
        {
            u.normalize();
        }

        const Vector3f r = d.cross(u);

        // The matrix that rotates [r0, u0, d0] into [r, u, d]
        // is defined below
        Matrix3f rotation;
        rotation << r, u, -d;

        _orientation = rotation;
    }
}
