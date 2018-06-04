#include "renderer.hpp"

using Eigen::Quaternionf;
using Eigen::Vector3f;
using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;
using Eigen::Translation3f;
using Eigen::Scaling;

namespace merely3d
{
    Renderer Renderer::build()
    {
        return Renderer(ShaderCollection::create_in_context(),
                        TrianglePrimitiveRenderer::build(),
                        MeshRenderer::build(),
                        GlLine::create());
    }

    void Renderer::render(CommandBuffer & buffer,
                          const Camera & camera,
                          const Matrix4f & projection)
    {
        // TODO: Make clear color configurable
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        primitive_renderer.render(shader_collection, buffer, camera, projection);
        mesh_renderer.render(shader_collection, buffer, camera, projection);

        // TODO: Create a LineRenderer class or similar to encapsulate
        // line rendering
        gl_line.bind();

        auto & line_shader = shader_collection.line_shader();

        line_shader.use();

        const Affine3f view = camera.transform().inverse();

        const Vector3f e1 = Vector3f::UnitX();

        for (const auto & line : buffer.lines())
        {
            const Eigen::Vector3f c = line.to - line.from;

            // The reference line is defined to be e1, i.e. [1, 0, 0], so we need the
            // rotation which rotates e1 into the direction of c.
            const Quaternionf rotation = Quaternionf::FromTwoVectors(e1, c);
            const Eigen::Affine3f model = Eigen::Translation3f(line.from) * rotation * Eigen::Scaling(c.norm());

            line_shader.set_projection_transform(projection);
            line_shader.set_view_transform(view);
            line_shader.set_model_transform(model);
            line_shader.set_object_color(line.color);
            glDrawArrays(GL_LINES, 0, 2);
        }

    }
}
