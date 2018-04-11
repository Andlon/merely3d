#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <merely3d/camera.hpp>

#include "shader.hpp"
#include "command_buffer.hpp"
#include "gl_primitive.hpp"

namespace merely3d
{
    // TODO: Cleanup of resources
    class Renderer
    {
    public:
        Renderer(const Renderer & other) = delete;
        Renderer(Renderer && other) = default;

        void render(const CommandBuffer & buffer,
                    const Camera & camera,
                    int viewport_width,
                    int viewport_height);

        static Renderer build();

    private:
        Renderer(ShaderProgram program,
                 GlPrimitive gl_cube,
                 GlPrimitive gl_rectangle,
                 GlPrimitive gl_sphere);

        ShaderProgram default_program;

        GlPrimitive gl_rectangle;
        GlPrimitive gl_cube;
        GlPrimitive gl_sphere;

        size_t num_sphere_vertices;
    };

}
