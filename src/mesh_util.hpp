#pragma once

#include <cassert>
#include <cstdlib>
#include <vector>

#include <Eigen/Dense>

namespace merely3d
{
    struct Triangle
    {
        Eigen::Vector3f a;
        Eigen::Vector3f b;
        Eigen::Vector3f c;

        Triangle(const Eigen::Vector3f & a,
                 const Eigen::Vector3f & b,
                 const Eigen::Vector3f & c)
            : a(a), b(b), c(c)
        {}
    };

    inline void push_vertex(std::vector<float> & c, const Eigen::Vector3f & v)
    {
        c.push_back(v(0));
        c.push_back(v(1));
        c.push_back(v(2));
    }

    inline std::vector<float> replicate_vertices(const std::vector<float> & vertices,
                                          const std::vector<int> & indices)
    {
        assert(indices.size() % 3 == 0);

        std::vector<float> result;

        const auto & v = vertices;

        for (const auto & i : indices)
        {
            const auto offset = 3 * i;
            result.push_back(v[offset + 0]);
            result.push_back(v[offset + 1]);
            result.push_back(v[offset + 2]);
        }

        return result;
    }

    inline std::vector<float> subdivide(const std::vector<float> & replicated_vertices)
    {
        assert(replicated_vertices.size() % 9 == 0);
        using Eigen::Vector3f;

        std::vector<float> result;

        const auto & v = replicated_vertices;
        const auto num_vertices = replicated_vertices.size() / 3;
        const auto num_triangles = num_vertices / 3;

        for (size_t t = 0; t < num_triangles; ++t)
        {
            const auto offset = 9 * t;
            const auto a = Vector3f(v[offset + 0], v[offset + 1], v[offset + 2]);
            const auto b = Vector3f(v[offset + 3], v[offset + 4], v[offset + 5]);
            const auto c = Vector3f(v[offset + 6], v[offset + 7], v[offset + 8]);

            // Compute midpoints
            const Vector3f ab = 0.5 * (a + b);
            const Vector3f bc = 0.5 * (b + c);
            const Vector3f ca = 0.5 * (c + a);

            // We get new triangles
            // a - ab - ca
            // ab - b - bc
            // bc - c - ca
            // ab - bc - ca
            const Triangle triangles[] = {
                Triangle(a, ab, ca),
                Triangle(ab, b, bc),
                Triangle(bc, c, ca),
                Triangle(ab, bc, ca)
            };

            for (const auto & tri : triangles)
            {
                push_vertex(result, tri.a);
                push_vertex(result, tri.b);
                push_vertex(result, tri.c);
            }
        }

        return result;
    }

    /// Returns the vertices of the unit rectangle,
    /// defined to lie in the xy-plane (z = 0),
    /// centered at (0, 0, 0) and with unit length sides.
    inline std::vector<float> unit_rectangle_vertices_and_normals()
    {
        // We render both faces with opposite normals
        return {
             // Front
             0.5, -0.5, 0.0, 0.0, 0.0, 1.0,
             0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5, -0.5, 0.0, 0.0, 0.0, 1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, 1.0,

             // Back
            -0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
             0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
            -0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
        };
    }

    /// Returns the vertices and normals of the unit rectangle,
    /// defined to lie in the xy-plane (z = 0),
    /// centered at (0, 0, 0) and with unit length sides.
    ///
    /// The data is organized follows:
    /// { v1_x, v1_y, v1_z, n1_x, n1_y, n1_z, v2_x, ... }
    /// where v1 is vertex 1, n1 is the normal of vertex 1 and so on.
    inline std::vector<float> unit_cube_vertices_and_normals()
    {
        // vertices and normals courtesy of learnopengl.com
        return {
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
        };
    }

    inline std::vector<float> unit_icosahedron_vertices()
    {
        // The vertex coordinates are given by the cyclic permutations of (0, +-1, +- phi)
        // Here we have used the ordering of the vertices and the triangle indexing
        // provided by Andreas Kahler on his blog:
        // http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html

        const float phi = (1.0f + std::sqrt(5.0f)) / 2.0f;
        return {
                -1.0, phi, 0.0,
                1.0, phi, 0.0,
                -1.0, -phi, 0.0,
                1.0, -phi, 0.0,

                0.0, -1.0, phi,
                0.0, 1.0, phi,
                0.0, -1.0, -phi,
                0.0, 1.0, -phi,

                phi, 0.0, -1.0,
                phi, 0.0, 1.0,
                -phi, 0.0, -1.0,
                -phi, 0.0, 1.0
        };
    }

    inline std::vector<int> unit_icosahedron_indices()
    {
        return {
                0, 11, 5,
                0, 5, 1,
                0, 1, 7,
                0, 7, 10,
                0, 10, 11,

                1, 5, 9,
                5, 11, 4,
                11, 10, 2,
                10, 7, 6,
                7, 1, 8,

                3, 9, 4,
                3, 4, 2,
                3, 2, 6,
                3, 6, 8,
                3, 8, 9,

                4, 9, 5,
                2, 4, 11,
                6, 2, 10,
                8, 6, 7,
                9, 8, 1
        };
    }

    inline std::vector<float> unit_sphere_vertices_and_normals(unsigned int num_subdivisions = 4)
    {
        using Eigen::Vector3f;

        const auto icosahedron_verts = unit_icosahedron_vertices();
        const auto icosahedron_idx = unit_icosahedron_indices();
        auto v = merely3d::replicate_vertices(icosahedron_verts, icosahedron_idx);

        assert(v.size() % 9 == 0);

        for (unsigned int s = 0; s < num_subdivisions; ++s)
        {
            v = subdivide(v);
        }

        assert(v.size() % 9 == 0);
        const auto num_triangles = v.size() / 9;

        std::vector<float> result;
        for (size_t t = 0; t < num_triangles; ++t)
        {
            const auto offset = 9 * t;
            auto a = Vector3f(v[offset + 0], v[offset + 1], v[offset + 2]);
            auto b = Vector3f(v[offset + 3], v[offset + 4], v[offset + 5]);
            auto c = Vector3f(v[offset + 6], v[offset + 7], v[offset + 8]);
            a.normalize();
            b.normalize();
            c.normalize();

            // Normal is exactly equal to vertex, hence we push twice
            push_vertex(result, a);
            push_vertex(result, a);
            push_vertex(result, b);
            push_vertex(result, b);
            push_vertex(result, c);
            push_vertex(result, c);
        }

        return result;
    }

}
