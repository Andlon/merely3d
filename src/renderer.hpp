#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <merely3d/camera.hpp>

#include "shader.hpp"
#include "command_buffer.hpp"

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
        Renderer(ShaderProgram program);

        ShaderProgram default_program;

        GLuint rectangle_vbo;
        GLuint rectangle_vao;
        GLuint rectangle_ebo;
    };

}