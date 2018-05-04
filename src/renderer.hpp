#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <merely3d/camera.hpp>

#include "shader.hpp"
#include "command_buffer.hpp"
#include "gl_primitive.hpp"
#include "gl_line.hpp"

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
        Renderer(ShaderProgram && line_program,
                 TrianglePrimitiveRenderer && primitive_renderer,
                 GlLine && gl_line)
            : line_program(std::move(line_program)),
              primitive_renderer(std::move(primitive_renderer)),
              gl_line(std::move(gl_line))
        {}


        ShaderProgram line_program;

        TrianglePrimitiveRenderer primitive_renderer;

        GlLine      gl_line;
    };

}
