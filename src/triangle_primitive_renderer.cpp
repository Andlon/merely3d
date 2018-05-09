#include "triangle_primitive_renderer.hpp"
#include "mesh_util.hpp"

#include <shaders.hpp>

#include <Eigen/Dense>

using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::Scaling;
using Eigen::Translation3f;

namespace merely3d
{
    /// Computes the model matrix for a renderable
    /// and a transform which transforms the reference
    /// shape into the given shape in its local coordinate system
    /// (for example, transform the unit cube into an axis-aligned box)
    template <typename Shape, typename Transform>
    static Eigen::Affine3f model_transform_from_reference(
        const Renderable<Shape> & renderable,
        const Transform & reference_transform)
    {
        return Translation3f(renderable.position)
               * renderable.orientation
               * Scaling(renderable.scale)
               * reference_transform;
    }

    template <typename It>
    It partition_wireframe_shapes(It begin, It end)
    {
        return std::partition(begin, end, [] (const typename It::value_type & shape)
        {
            return shape.material.wireframe;
        });
    }

    TrianglePrimitiveRenderer TrianglePrimitiveRenderer::build()
    {
        const auto cube_verts = unit_cube_vertices_and_normals();
        auto gl_cube = GlPrimitive::create(cube_verts);

        const auto rect_verts = unit_rectangle_vertices_and_normals();
        auto gl_rect = GlPrimitive::create(rect_verts);

        const auto sphere_verts = unit_sphere_vertices_and_normals();
        auto gl_sphere = GlPrimitive::create(sphere_verts);

        return TrianglePrimitiveRenderer(std::move(gl_cube),
                                         std::move(gl_rect),
                                         std::move(gl_sphere));
    }

    void TrianglePrimitiveRenderer::render(
                ShaderCollection & shaders,
                CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection)
    {
        auto & mesh_shader = shaders.mesh_shader();
        auto & line_shader = shaders.line_shader();

        const Eigen::Affine3f view = camera.transform().inverse();

        // TODO: Make lighting configurable rather than hard-coded
        const auto light_color = Color(1.0, 1.0, 1.0);
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        mesh_shader.use();

        auto draw_filled_primitive = [&] (const GlPrimitive & primitive,
                                          const Affine3f & model,
                                          const Material & material)
        {
            const auto normal_transform = Matrix3f(model.linear().inverse().transpose());

            mesh_shader.use();
            // TODO: Move light/camera/projection/view etc. out of
            // inner rendering loop
            mesh_shader.set_light_color(light_color);
            mesh_shader.set_light_direction(light_dir);
            mesh_shader.set_view_transform(view);
            mesh_shader.set_projection_transform(projection);
            mesh_shader.set_model_transform(model);
            mesh_shader.set_normal_transform(normal_transform);
            mesh_shader.set_object_color(material.color);
            mesh_shader.set_camera_position(camera.position());
            glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
        };

        auto draw_wireframe_primitive = [&] (const GlPrimitive & primitive,
                                             const Affine3f & model,
                                             const Material & material)
        {
            line_shader.use();
            // TODO: Move projection/view transforms out of individual
            // object rendering
            line_shader.set_projection_transform(projection);
            line_shader.set_view_transform(view);
            line_shader.set_model_transform(model);
            line_shader.set_object_color(material.color);
            glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
        };

        auto draw_primitive = [&] (const GlPrimitive & primitive,
                                             const Affine3f & model,
                                             const Material & material)
        {
            if (material.wireframe) draw_wireframe_primitive(primitive, model, material);
            else                    draw_filled_primitive(primitive, model, material);
        };

        gl_rectangle.bind();

        // Face culling is necessary to properly render rectangles from both sides
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        auto filled_begin = partition_wireframe_shapes(buffer.rectangles().begin(), buffer.rectangles().end());

        line_shader.use();
        enable_wireframe_rendering(true);
        for (auto it = buffer.rectangles().begin(); it != filled_begin; ++it)
        {
            const auto & rectangle = *it;
            const auto & extents = rectangle.shape.extents;
            const auto reference_transform = Scaling(extents.x(), extents.y(), 1.0f);
            const auto model = model_transform_from_reference(rectangle, reference_transform);
            draw_wireframe_primitive(gl_rectangle, model, rectangle.material);
        }

        mesh_shader.use();
        enable_wireframe_rendering(false);
        for (auto it = filled_begin; it != buffer.rectangles().end(); ++it)
        {
            const auto & rectangle = *it;
            const auto & extents = rectangle.shape.extents;
            const auto reference_transform = Scaling(extents.x(), extents.y(), 1.0f);
            const auto model = model_transform_from_reference(rectangle, reference_transform);
            draw_filled_primitive(gl_rectangle, model, rectangle.material);
        }

        gl_cube.bind();
        glDisable(GL_CULL_FACE);

        for (const auto & box : buffer.boxes())
        {
            const auto & extents = box.shape.extents;
            const auto reference_transform = Scaling(extents);
            const auto model = model_transform_from_reference(box, reference_transform);
            draw_primitive(gl_cube, model, box.material);
        }

        gl_sphere.bind();

        for (const auto & sphere : buffer.spheres())
        {
            const auto reference_transform = Eigen::Scaling(sphere.shape.radius);
            const auto model = model_transform_from_reference(sphere, reference_transform);
            draw_primitive(gl_sphere, model, sphere.material);
        }
    }

    void TrianglePrimitiveRenderer::enable_wireframe_rendering(bool enable)
    {
        if (enable)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    }
}
