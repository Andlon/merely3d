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
            // TODO: Make atomic
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

            StaticMeshData() = delete;
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

        static StaticMesh with_angle_weighted_normals(std::vector<float> vertices, std::vector<unsigned int> faces);

    private:
        // Allocate the mesh data on the heap, so that we have a stable address
        // even if the user chooses to move the StaticMesh instance.
        std::shared_ptr<const detail::StaticMeshData> _data;

        friend class MeshRenderer;
    };

}
