#include <merely3d/mesh.hpp>

#include <Eigen/Dense>

using Eigen::Vector3f;

namespace merely3d
{
    StaticMesh::StaticMesh(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces)
            : _data(new detail::StaticMeshData(std::move(vertices_and_normals),
                                               std::move(faces)))
    {
        if (_data->faces.size() % 3 != 0)
        {
            throw std::invalid_argument("Faces must have size divisible by 3");
        }
        if (_data->vertices_and_normals.size() % 6 != 0)
        {
            throw std::invalid_argument("Vertices and normals must have size divisible by 6");
        }
    }

    StaticMesh::StaticMesh(std::vector<float> vertices,
                                  std::vector<float> normals,
                                  std::vector<unsigned int> faces)
    {
        if (vertices.size() % 3 != 0)
        {
            throw std::invalid_argument("Vertices must have size divisible by 3");
        }
        if (normals.size() % 3 != 0)
        {
            throw std::invalid_argument("Normals must have size divisible by 3");
        }
        if (faces.size() % 3 != 0)
        {
            throw std::invalid_argument("Faces must have size divisible by 3");
        }
        if (vertices.size() != normals.size())
        {
            throw std::invalid_argument("Number of vertices and normals must be the same.");
        }

        const auto num_vertices = vertices.size() / 3;
        decltype(vertices) vertices_and_normals;
        auto & vn = vertices_and_normals;

        for (size_t i = 0; i < num_vertices; ++i)
        {
            const auto vbegin = vertices.begin() + 3 * i;
            const auto nbegin = normals.begin() + 3 * i;
            vn.insert(vn.end(), vbegin, vbegin + 3);
            vn.insert(vn.end(), nbegin, nbegin + 3);
        }
        _data = std::make_shared<detail::StaticMeshData>(std::move(vertices_and_normals), std::move(faces));
    }

    StaticMesh StaticMesh::with_angle_weighted_normals(std::vector<float> vertices, std::vector<unsigned int> faces)
    {
        if (vertices.size() % 3 != 0)
        {
            throw std::invalid_argument("Vertices must have size divisible by 3");
        }
        if (faces.size() % 3 != 0)
        {
            throw std::invalid_argument("Faces must have size divisible by 3");
        }

        std::vector<float> normals(vertices.size(), 0.0f);

        for (size_t i = 0; i < faces.size(); i += 3)
        {
            const auto triangle = std::array<unsigned int, 3> { faces[i], faces[i + 1], faces[i + 2] };

            const auto v0 = Vector3f(vertices[3 * triangle[0]], vertices[3 * triangle[0] + 1], vertices[3 * triangle[0] + 2]);
            const auto v1 = Vector3f(vertices[3 * triangle[1]], vertices[3 * triangle[1] + 1], vertices[3 * triangle[1] + 2]);
            const auto v2 = Vector3f(vertices[3 * triangle[2]], vertices[3 * triangle[2] + 1], vertices[3 * triangle[2] + 2]);

            const auto face_vertices = std::array<Vector3f, 3> { v0, v1, v2 };
            const Vector3f face_normal = (v1 - v0).cross(v2 - v0).normalized();

            // For each vertex in the face, compute contribution to angle-weighted vertex normals
            for (size_t j = 0; j < 3; ++j)
            {
                // Compute left and right edge vectors that meet in local vertex (j + 1)
                const Vector3f left = (face_vertices[j] - face_vertices[(j + 1) % 3]).normalized();
                const Vector3f right = (face_vertices[(j + 2) % 3] - face_vertices[(j + 1) % 3]).normalized();
                const float dot = left.dot(right);
                // Rounding errors may push the dot product beyond the domain of acos, so we clamp the result
                // to the allowed range [-1, 1]
                const float angle = std::acos(std::max(-1.0f, std::min(dot, 1.0f)));
                const auto vertex_index = triangle[(j + 1) % 3];
                assert(vertex_index < vertices.size() / 3);
                assert(std::isfinite(angle));
                normals[3 * vertex_index + 0] += angle * face_normal[0];
                normals[3 * vertex_index + 1] += angle * face_normal[1];
                normals[3 * vertex_index + 2] += angle * face_normal[2];
            }
        }

        for (size_t i = 0; i < normals.size(); i += 3)
        {
            const Vector3f normal = Vector3f(normals[i], normals[i + 1], normals[i + 2]).normalized();
            normals[i + 0] = normal[0];
            normals[i + 1] = normal[1];
            normals[i + 2] = normal[2];
        }

        return StaticMesh(std::move(vertices), std::move(normals), std::move(faces));
    }
}