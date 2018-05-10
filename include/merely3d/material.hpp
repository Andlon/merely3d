#pragma once

#include <merely3d/color.hpp>

namespace merely3d
{
    const Color DEFAULT_MATERIAL_COLOR = Color(0.7, 0.7, 0.7);

    struct Material
    {
        Material() : color(DEFAULT_MATERIAL_COLOR),
                     wireframe(false),
                     pattern_grid_size(0.5f) {}

        Color color;

        // Whether or not to render this entity as a wireframe,
        // rather than a properly shaded mesh.
        bool wireframe;

        // Determines the side length of the cubes determining the pattern.
        // Set to 0.0f to disable patterned rendering.
        float pattern_grid_size;

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

        Material with_pattern_grid_size(float size)
        {
            auto result = *this;
            result.pattern_grid_size = size;
            return result;
        }
    };
}
