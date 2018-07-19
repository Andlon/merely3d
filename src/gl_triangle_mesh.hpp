#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cassert>

#include "gl_gc.hpp"

namespace merely3d
{
    /// Helper class for managing meshes represented as
    /// an indexed triangle list, i.e. the mesh is represented by a list of vertices
    /// and a list of triangles represented as triplets of indices into the vertex list.
    class GlTriangleMesh
    {
    public:
        GlTriangleMesh(GlTriangleMesh && other) noexcept
                :   vao(other.vao), vbo(other.vbo), ebo(other.ebo),
                    num_vertices(other.num_vertices), num_indices(other.num_indices),
                    garbage(other.garbage)
        {
            other.vao = 0;
            other.vbo = 0;
            other.ebo = 0;
            other.num_vertices = 0;
            other.num_indices = 0;
            other.garbage.reset();
        }

        ~GlTriangleMesh();

        GlTriangleMesh(const GlTriangleMesh & other) = delete;
        GlTriangleMesh & operator=(const GlTriangleMesh & other) = delete;
        GlTriangleMesh & operator=(GlTriangleMesh && other) = delete;

        /// Creates a new mesh allocated on the GPU, using the
        /// specified vertices and normals, in the format
        /// { v1_x, v1_y, v1_z, n1_x, n1_y, n1_z, v2_x, ...},
        /// and triangles represented by an index list
        /// { t1_a, t1_b, t1_c, t2_a, t2_b, t2_c, ... },
        /// where each ti_a, ti_b, ti_c correspond to the indices of the a, b, c vertices
        /// that make up the triangle.
        ///
        /// Note that the correct OpenGL context MUST be set prior to
        /// calling this function.
        ///
        /// Shader attributes are set up according to the format
        /// description that was just presented.
        static GlTriangleMesh create(const std::shared_ptr<GlGarbagePile> & garbage,
                                     const std::vector<float> & vertices_and_normals,
                                     const std::vector<unsigned int> &triangles);

        /// Binds the associated buffers of this mesh.
        void bind();

        /// Unbinds the associated buffers of this mesh.
        void unbind();

        size_t vertex_count() const
        {
            return num_vertices;
        }

        size_t triangle_count() const
        {
            return num_indices / 3;
        }

        size_t index_count() const
        {
            return num_indices;
        }

    private:
        GlTriangleMesh(const std::shared_ptr<GlGarbagePile> & garbage, GLuint vao, GLuint vbo, GLuint ebo, size_t num_vertices, size_t num_triangles)
                : vao(vao), vbo(vbo), ebo(ebo), num_vertices(num_vertices), num_indices(num_triangles * 3),
                  garbage(garbage)
        {
            assert(this->garbage);
            assert(num_triangles % 3 == 0);
        }

        GLuint vao;
        GLuint vbo;
        GLuint ebo;

        size_t num_vertices;
        size_t num_indices;

        std::shared_ptr<GlGarbagePile> garbage;
    };

    inline GlTriangleMesh GlTriangleMesh::create(const std::shared_ptr<GlGarbagePile> & garbage,
                                                 const std::vector<float> & vertices_and_normals,
                                                 const std::vector<unsigned int> &triangles)
    {
        // Each vertex is represented by 3 floats for position and 3 floats for its associated normal
        assert(vertices_and_normals.size() % 6 == 0);

        // Each triangle is represented by a triplet of indices
        assert(triangles.size() % 3 == 0);

        const auto num_vertices = vertices_and_normals.size() / 6;
        const auto num_triangles = triangles.size();

        const auto & v = vertices_and_normals.data();
        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_and_normals.size(), v, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(), triangles.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        return { garbage, vao, vbo, ebo, num_vertices, num_triangles };
    }

    inline void GlTriangleMesh::bind()
    {
        glBindVertexArray(vao);
    }

    inline void GlTriangleMesh::unbind()
    {
        glBindVertexArray(0);
    }

    inline GlTriangleMesh::~GlTriangleMesh()
    {
        if (garbage)
        {
            garbage->delete_element_buffer_later(ebo);
            garbage->delete_vertex_buffer_later(vbo);
            garbage->delete_vertex_array_later(vao);
        }
    }
}
