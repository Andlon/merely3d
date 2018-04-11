#pragma once

#include <array>

namespace merely3d
{
    class Color final
    {
    public:
        Color(float r, float g, float b)
            : _r(r), _g(g), _b(b) {};

        float r() const { return _r; }
        float g() const { return _g; }
        float b() const { return _b; }

        std::array<float, 3> into_array() const
        {
            return { r(), g(), b() };
        }

    private:
        float _r;
        float _g;
        float _b;
    };

    inline Color red()
    {
        return { 1.0f, 0.0f, 0.0f };
    }

    inline Color green()
    {
        return { 0.0f, 1.0f, 0.0f };
    }

    inline Color blue()
    {
        return { 0.0f, 0.0f, 1.0f };
    }
}
