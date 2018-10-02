#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <merely3d/camera.hpp>

#include "command_buffer.hpp"
#include "gl_line.hpp"
#include "shader_collection.hpp"

#include "renderers.hpp"
#include "gl_gc.hpp"

namespace merely3d
{
    // TODO: Cleanup of resources
    class Renderer
    {
    public:
        Renderer(const Renderer & other) = delete;
        Renderer(Renderer && other) = default;

        void render(CommandBuffer & buffer,
                    const Camera & camera,
                    const Eigen::Matrix4f & projection);

        static Renderer build();

    private:
        Renderer(ShaderCollection && shader_collection,
                 TrianglePrimitiveRenderer && primitive_renderer,
                 MeshRenderer && mesh_renderer,
                 ParticleRenderer && particle_renderer,
                 GlLine && gl_line,
                 GlGarbageCollector && gc)
            : shader_collection(std::move(shader_collection)),
              primitive_renderer(std::move(primitive_renderer)),
              mesh_renderer(std::move(mesh_renderer)),
              particle_renderer(std::move(particle_renderer)),
              gl_line(std::move(gl_line)),
              gc(std::move(gc))
        {}

        ShaderCollection            shader_collection;
        TrianglePrimitiveRenderer   primitive_renderer;
        MeshRenderer                mesh_renderer;
        ParticleRenderer            particle_renderer;
        GlLine                      gl_line;
        GlGarbageCollector          gc;
    };

}
