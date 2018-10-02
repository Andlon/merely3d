#pragma once

#include "gl_line.hpp"
#include "gl_primitive.hpp"
#include "gl_triangle_mesh.hpp"
#include "gl_particle_buffer.hpp"
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

    static MeshRenderer build(const std::shared_ptr<GlGarbagePile> & garbage);

private:
    MeshRenderer(const std::shared_ptr<GlGarbagePile> & garbage)
        : _garbage(garbage) { }

    std::unordered_map<detail::UniqueMeshId, GlTriangleMesh> _mesh_cache;
    std::shared_ptr<GlGarbagePile>                           _garbage;
};

class ParticleRenderer
{
public:
    void render(ShaderCollection & shaders,
                CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection);

    static ParticleRenderer build(const std::shared_ptr<GlGarbagePile> & garbage);

private:
    ParticleRenderer(GlParticleBuffer && buffer)
        : _particle_buffer(std::move(buffer)) { }

    GlParticleBuffer                    _particle_buffer;
};

}
