#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace merely3d
{
    class GlLine
    {
    public:
        GlLine(GlLine && other)
            : vao(other.vao), vbo(other.vbo)
        {
            other.vao = 0;
            other.vbo = 0;
        }

        GlLine(const GlLine & other) = delete;
        GlLine & operator=(const GlLine & other) = delete;

        static GlLine create();

        void bind();
        void unbind();

    private:
        GlLine(GLuint vao, GLuint vbo)
                : vao(vao), vbo(vbo)
        {}

        GLuint vao;
        GLuint vbo;
    };

    inline GlLine GlLine::create()
    {
        GLuint vao, vbo;

        const float buffer[] = { 0.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, buffer, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        return { vao, vbo };
    }

    inline void GlLine::bind()
    {
        glBindVertexArray(vao);
    }

    inline void GlLine::unbind()
    {
        glBindVertexArray(vbo);
    }
}