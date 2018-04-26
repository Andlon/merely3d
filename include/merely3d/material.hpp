#pragma once

#include <merely3d/color.hpp>

namespace merely3d
{
    const Color DEFAULT_MATERIAL_COLOR = Color(0.7, 0.7, 0.7);

    struct Material
    {
        Material() : color(DEFAULT_MATERIAL_COLOR), wireframe(false) {}

        Color color;
        bool wireframe;

        Material with_color(const Color & color) const
        {
            auto result = *this;
            result.color = color;
            return result;
        }

        Material with_wireframe(bool wireframe) const
        {
            auto result = *this;
            result.wireframe = wireframe;
            return result;
        }
    };
}
