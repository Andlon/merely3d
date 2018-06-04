#pragma once

#include "gl_line.hpp"
#include "gl_primitive.hpp"
#include "gl_triangle_mesh.hpp"
#include "shader.hpp"
#include "shader_collection.hpp"
#include "command_buffer.hpp"

#include <merely3d/camera.hpp>
#include <merely3d/mesh.hpp>

#include <vector>
#include <unordered_map>

namespace merely3d
{

class TrianglePrimitiveRenderer
{
public:
    void render(ShaderCollection & shaders,
                CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection);

    static TrianglePrimitiveRenderer build();

private:

    TrianglePrimitiveRenderer(GlPrimitive && gl_cube,
                              GlPrimitive && gl_rectangle,
                              GlPrimitive && gl_sphere)
        : gl_cube(std::move(gl_cube)),
          gl_rectangle(std::move(gl_rectangle)),
          gl_sphere(std::move(gl_sphere))
    {}

    GlPrimitive gl_cube;
    GlPrimitive gl_rectangle;
    GlPrimitive gl_sphere;
};

class MeshRenderer
{
public:
    void render(ShaderCollection & shaders,
                CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection);

    static MeshRenderer build();

private:
    MeshRenderer() {}

    std::unordered_map<const detail::StaticMeshData *, GlTriangleMesh> _mesh_cache;
};

}
