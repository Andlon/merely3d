#include <Eigen/Dense>

#include <iostream>

namespace merely3d
{
    class Camera
    {
    public:
        Camera();

        Eigen::Vector3f position() const;
        Eigen::Vector3f direction() const;
        Eigen::Vector3f up() const;

        Eigen::Affine3f transform() const;

        void set_position(const Eigen::Vector3f & position);

        template <typename DerivedRotation>
        void set_orientation(const Eigen::RotationBase<DerivedRotation, 3> & orientation);

        void look_in(const Eigen::Vector3f & direction,
                     const Eigen::Vector3f & up);

    private:
        Eigen::Affine3f _transform;
    };

    inline Camera::Camera()
        : _transform(Eigen::Affine3f::Identity())
    {

    }

    inline Eigen::Vector3f Camera::position() const
    {
        return _transform.translation();
    }

    inline Eigen::Vector3f Camera::direction() const
    {
        // Note: we assume that the linear part of the transform is a pure
        // rotation, which should be the case since the Camera cannot be
        // constructed from anything which would include a scaling or
        // reflection
        return _transform.linear() * Eigen::Vector3f(0.0, 0.0, -1.0);
    }

    inline Eigen::Vector3f Camera::up() const
    {
        // See note for direction()
        return _transform.linear() * Eigen::Vector3f(0.0, 1.0, 0.0);
    }

    inline Eigen::Affine3f Camera::transform() const
    {
        return _transform;
    }

    inline void Camera::set_position(const Eigen::Vector3f & position)
    {
        _transform.translation() = position;
    }

    template <typename DerivedRotation>
    inline void Camera::set_orientation(const Eigen::RotationBase<DerivedRotation, 3> & orientation)
    {
        _transform.linear() = orientation;
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

        std::cout << std::endl << rotation << std::endl;

        _transform.linear() = rotation;
    }
}