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

    static const Color DEFAULT_PARTICLE_COLOR = Color(0.1, 0.1, 0.7);
    static float DEFAULT_PARTICLE_RADIUS = 0.2;

    struct Particle
    {
        Particle() : Particle(Eigen::Vector3f::Zero()) {}
        Particle(const Eigen::Vector3f & position,
                 float radius = DEFAULT_PARTICLE_RADIUS,
                 const Color & color = DEFAULT_PARTICLE_COLOR)
            : position(position), color(color), radius(radius) {}
        Particle(float x, float y, float z,
                 float radius = DEFAULT_PARTICLE_RADIUS,
                 const Color & color = DEFAULT_PARTICLE_COLOR)
            : Particle(Eigen::Vector3f(x, y, z), radius, color)
        {}

        Eigen::Vector3f position;
        Color           color;
        float           radius;

        Particle with_radius(float new_radius) const
        {
            return Particle(position, new_radius, color);
        }

        Particle with_position(const Eigen::Vector3f & new_position) const
        {
            return Particle(new_position, radius, color);
        }

        Particle with_position(float x, float y, float z) const
        {
            return Particle(x, y, z, radius, color);
        }

        Particle with_color(const Color & new_color) const
        {
            return Particle(position, radius, new_color);
        }
    };
}
