#pragma once

#include <vector>
#include <memory>

namespace merely3d
{
    namespace detail
    {
        struct StaticMeshData
        {
            std::vector<float> vertices_and_normals;
            std::vector<unsigned int> faces;

            StaticMeshData(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces)
                    : vertices_and_normals(std::move(vertices_and_normals)), faces(std::move(faces))
            {}
        };
    }

    /**
     * Represents a static mesh.
     *
     * Typically, you create this once and hand it to Frame.draw() on every frame render.
     * merely3d internally transfers the data to the GPU only when needed.
     */
    class StaticMesh
    {
    public:
        StaticMesh(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces);

    private:
        // Allocate the mesh data on the heap, so that we have a stable address
        // even if the user chooses to move the StaticMesh instance.
        std::shared_ptr<const detail::StaticMeshData> _data;

        friend class MeshRenderer;
    };

    inline StaticMesh::StaticMesh(std::vector<float> vertices_and_normals, std::vector<unsigned int> faces)
        : _data(new detail::StaticMeshData(std::forward<std::vector<float>>(vertices_and_normals),
                                           std::forward<std::vector<unsigned int>>(faces)))
    { }


}
