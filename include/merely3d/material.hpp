#pragma once

#include <merely3d/color.hpp>

namespace merely3d
{
    const Color DEFAULT_MATERIAL_COLOR = Color(0.7, 0.7, 0.7);

    struct Material
    {
        Material() : color(DEFAULT_MATERIAL_COLOR) {}

        Color color;

        Material with_color(const Color & color) const
        {
            auto result = *this;
            result.color = color;
            return result;
        }
    };
}
