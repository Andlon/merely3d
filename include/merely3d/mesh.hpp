#pragma once

#include <vector>
#include <memory>

namespace merely3d
{
    namespace detail
    {
        typedef uint64_t UniqueMeshId;

        inline UniqueMeshId next_mesh_id()
        {
            static UniqueMeshId next_id = 0;
            return next_id++;
        }

        struct StaticMeshData
        {
            std::vector<float> vertices_and_normals;
            std::vector<unsigned int> faces;

            /// Globally unique ID for this mesh data. Needed for correct caching of mesh data.
            /// Previously we used pointers, but in the case of frequent de- and reallocation,
            /// different data may be placed at the exact same address, which makes the pointer-based approach
            /// unreliable.
            const UniqueMeshId id;

            StaticMeshData() = default;
            StaticMeshData(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces)
                    : vertices_and_normals(std::move(vertices_and_normals)), faces(std::move(faces)),
                      id(next_mesh_id())
            {}
        };
    }

    /**
     * Represents a static mesh.
     *
     * Typically, you create this once and hand it to Frame.draw() on every frame render.
     * merely3d internally transfers the data to the GPU only when needed.
     *
     */
    class StaticMesh
    {
    public:
        /// Create a static mesh from a set of vertices, accompanying normals and a list of faces.
        ///
        /// Vertices and normals are represented as groups of 3 floating point numbers.
        /// That is, vertices.size() and normals.size() must be equal. Similarly, faces are represented by
        /// groups of 3 indices, which represent the index of each vertex of the face.
        StaticMesh(std::vector<float> vertices, std::vector<float> normals, std::vector<unsigned int> faces);
        StaticMesh(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces);

    private:
        // Allocate the mesh data on the heap, so that we have a stable address
        // even if the user chooses to move the StaticMesh instance.
        std::shared_ptr<const detail::StaticMeshData> _data;

        friend class MeshRenderer;
    };

    inline StaticMesh::StaticMesh(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces)
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

    inline StaticMesh::StaticMesh(std::vector<float> vertices,
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


}
