#pragma once

#include <Eigen/Dense>

#include <merely3d/material.hpp>
#include <merely3d/types.hpp>

namespace merely3d
{
    template <typename Shape>
    struct Renderable
    {
        Shape                   shape;
        Eigen::Vector3f         position;
        UnalignedQuaternionf    orientation;
        Eigen::Vector3f         scale;
        Material                material;

        Renderable(Shape shape)
                : shape(std::move(shape)),
                  position(Eigen::Vector3f::Zero()),
                  orientation(Eigen::Quaternionf::Identity()),
                  scale(Eigen::Vector3f(1.0f, 1.0f, 1.0f))
        {}

        Renderable(Shape shape,
                   const Eigen::Vector3f & position,
                   const Eigen::Quaternionf & orientation,
                   const Eigen::Vector3f & scale,
                   Material material)
                : shape(std::move(shape)),
                  position(position),
                  orientation(orientation),
                  scale(scale),
                  material(material)
        {}

        template <typename OtherShape>
        Renderable<OtherShape> with_shape(OtherShape shape)
        {
            auto result = Renderable<OtherShape>(std::move(shape));
            result.position = position;
            result.orientation = orientation;
            result.scale = scale;
            result.material = material;
            return result;
        }

        Renderable<Shape> with_position(const Eigen::Vector3f & position)
        {
            auto result = *this;
            result.position = position;
            return result;
        }

        Renderable<Shape> with_position(float x, float y, float z)
        {
            return with_position(Eigen::Vector3f(x, y, z));
        }

        template <typename IntoOrientation>
        Renderable<Shape> with_orientation(const IntoOrientation & orientation)
        {
            auto result = *this;
            result.orientation = orientation;
            return result;
        }

        Renderable<Shape> with_scale(const Eigen::Vector3f & scale)
        {
            auto result = *this;
            result.scale = scale;
            return result;
        }

        Renderable<Shape> with_scale(float x_scale, float y_scale, float z_scale)
        {
            return with_scale(Eigen::Vector3f(x_scale, y_scale, z_scale));
        }

        Renderable<Shape> with_uniform_scale(float scale)
        {
            return with_scale(scale, scale, scale);
        }

        Renderable<Shape> with_material(const Material & material)
        {
            auto result = *this;
            result.material = material;
            return result;
        }
    };

    template <typename Shape>
    inline Renderable<Shape> renderable(Shape shape)
    {
        return Renderable<Shape>(std::move(shape));
    }
}