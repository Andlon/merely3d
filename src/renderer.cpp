#include "renderer.hpp"
#include "gl_primitive.hpp"
#include "mesh_util.hpp"

#include <shaders.hpp>

using Eigen::Quaternionf;
using Eigen::Vector3f;
using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Translation3f;
using Eigen::Scaling;

namespace
{
    /// Computes the "infinite" projection matrix with the given
    /// vertical field of view `fovy`, aspect ratio and the distance to
    /// the near plane.
    ///
    /// The projection matrix maps points from the view space in which
    /// negative z is "in front of" the camera. In other words,
    /// positive z values are not shown.
    Eigen::Matrix4f projection_matrix(float fovy, float aspect_ratio, float near_plane_dist)
    {
        assert(fovy > 0.0);
        assert(aspect_ratio > 0.0);
        assert(near_plane_dist > 0.0);
        const auto n = near_plane_dist;

        // Note: this is the perspective matrix with the far plane infinitely far away.
        // It will likely suffer some depth precision issues at large distances.
        // An interesting way to remedy this would be to follow the suggestion in the
        // following blogpost: https://chaosinmotion.blog/2010/09/06/goodbye-far-clipping-plane/
        // Essentially, there it's advocated to use a projection matrix which projects the far plane
        // (which is infinitely far away) onto z = 0, rather than z = 1. Floating point numbers are
        // able to represent numbers close to 0 much better than numbers close to 1,
        // so the end effect is a much more precise result. This would require setting the
        // clip plane in OpenGL through glClipPlane, however.
        Eigen::Matrix4f p;
        p << (fovy / aspect_ratio),  0.0,  0.0,      0.0,
                            0.0, fovy,  0.0,      0.0,
                            0.0,  0.0, -1.0, -2.0 * n,
                            0.0,  0.0, -1.0,      0.0;
        return p;
    }
}

namespace merely3d
{
    Renderer Renderer::build()
    {
        return Renderer(ShaderCollection::create_in_context(),
                        TrianglePrimitiveRenderer::build(),
                        GlLine::create());
    }

    void Renderer::render(CommandBuffer & buffer,
                          const Camera & camera,
                          int viewport_width,
                          int viewport_height)
    {
        // TODO: Make clear color configurable
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Move aspect ratio computation to separate function
        const auto width = static_cast<float>(viewport_width);
        const auto height = static_cast<float>(viewport_height);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = width > 0.0 && height > 0.0
                                  ? width / height
                                  : 1.0;
        const auto projection = projection_matrix(camera.fovy(), aspect_ratio, 0.1);

        primitive_renderer.render(shader_collection, buffer, camera, projection);

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
