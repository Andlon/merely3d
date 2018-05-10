#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <merely3d/camera.hpp>

#include "command_buffer.hpp"
#include "gl_line.hpp"
#include "shader_collection.hpp"

#include "triangle_primitive_renderer.hpp"

namespace merely3d
{
    // TODO: Cleanup of resources
    class Renderer
    {
    public:
        Renderer(const Renderer & other) = delete;
        Renderer(Renderer && other) = default;

        void render(CommandBuffer & buffer,
                    const Camera & camera,
                    int viewport_width,
                    int viewport_height);

        static Renderer build();

    private:
        Renderer(ShaderCollection && shader_collection,
                 TrianglePrimitiveRenderer && primitive_renderer,
                 GlLine && gl_line)
            : shader_collection(std::move(shader_collection)),
              primitive_renderer(std::move(primitive_renderer)),
              gl_line(std::move(gl_line))
        {}

        ShaderCollection shader_collection;
        TrianglePrimitiveRenderer primitive_renderer;
        GlLine      gl_line;
    };

}
