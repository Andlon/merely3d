#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cassert>

namespace merely3d
{
    /// Helper class for managing primitives represented as
    /// triangles with repeated vertices and associated normals.
    class GlPrimitive
    {
    public:
        GlPrimitive(GlPrimitive && other)
            :   vao(other.vao), vbo(other.vbo), num_vertices(other.num_vertices)
        {
            other.vao = 0;
            other.vbo = 0;
            other.num_vertices = 0;
        }

        GlPrimitive(const GlPrimitive & other) = delete;
        GlPrimitive & operator=(const GlPrimitive & other) = delete;

        /// Creates a new primitive allocated on the GPU, using the
        /// specified vertices and normals, in the format
        /// { v1_x, v1_y, v1_z, n1_x, n1_y, n1_z, v2_x, ...}
        /// Note that the correct OpenGL context MUST be set prior to
        /// calling this function.
        ///
        /// Shader attributes are set up according to the format
        /// description that was just presented.
        static GlPrimitive create(const std::vector<float> & vertices_and_normals);

        /// Binds the associated buffers of this primitive.
        void bind();

        /// Unbinds the associated buffers of this primitive.
        void unbind();

        size_t vertex_count() const
        {
            return num_vertices;
        }

    private:
        GlPrimitive(GLuint vao, GLuint vbo, size_t num_vertices)
            : vao(vao), vbo(vbo), num_vertices(num_vertices)
        {}

        GLuint vao;
        GLuint vbo;

        size_t num_vertices;
    };

    inline GlPrimitive GlPrimitive::create(const std::vector<float> & vertices_and_normals)
    {
        // Should be triangles of 3 vertices, each 3 floats,
        // plus 3 floats for each vertex corresponding to its normal vector
        assert(vertices_and_normals.size() % 18 == 0);

        const auto num_vertices = vertices_and_normals.size() / 6;

        const auto & v = vertices_and_normals.data();
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_and_normals.size(), v, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        return GlPrimitive(vao, vbo, num_vertices);
    }

    inline void GlPrimitive::bind()
    {
        glBindVertexArray(vao);
    }

    inline void GlPrimitive::unbind()
    {
        glBindVertexArray(0);
    }
}
