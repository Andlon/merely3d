#include "triangle_primitive_renderer.hpp"
#include "mesh_util.hpp"

#include <Eigen/Dense>

#include <algorithm>

using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Vector3f;
using Eigen::Scaling;
using Eigen::Translation3f;

namespace merely3d
{
    template <typename Shape>
    Affine3f build_model_transform(const Renderable<Shape> & renderable)
    {
        return Translation3f(renderable.position)
               * renderable.orientation
               * Scaling(renderable.scale);
    }

    static void enable_wireframe_rendering(bool enable)
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

    /// Render primitives.
    ///
    /// NB! Assumes that the uniforms not specific
    /// to the individual renderable are all correctly set.
    template <typename ReferenceTransform, typename Shape>
    void render_primitives(std::vector<Renderable<Shape>> & renderables,
                           ShaderCollection & shaders,
                           GlPrimitive & primitive,
                           ReferenceTransform && reference_transform)
    {
        primitive.bind();
        auto & line_shader = shaders.line_shader();
        auto & mesh_shader = shaders.mesh_shader();

        auto draw_primitive = [&] (const Renderable<Shape> & renderable)
        {
            const auto ref_transform = reference_transform(renderable.shape);
            const auto model = build_model_transform(renderable) * ref_transform;

            if (renderable.material.wireframe)
            {
                line_shader.set_model_transform(model);
                line_shader.set_object_color(renderable.material.color);
                glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
            }
            else
            {
                const auto normal_transform = Matrix3f(model.linear().inverse().transpose());
                mesh_shader.set_model_transform(model);
                mesh_shader.set_normal_transform(normal_transform);
                mesh_shader.set_object_color(renderable.material.color);
                mesh_shader.set_reference_transform(Matrix3f(ref_transform));
                mesh_shader.set_pattern_grid_size(std::max(0.0f, renderable.material.pattern_grid_size));
                glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
            }
        };

        // Partition vector so that renderables that are to be rendered as wireframes
        // come first
        auto filled_begin = std::partition(renderables.begin(), renderables.end(),
            [] (const Renderable<Shape> & renderable)
            {
                return renderable.material.wireframe;
            });

        line_shader.use();
        enable_wireframe_rendering(true);
        std::for_each(renderables.begin(), filled_begin, draw_primitive);

        mesh_shader.use();
        enable_wireframe_rendering(false);
        std::for_each(filled_begin, renderables.end(), draw_primitive);
    }

    /// Returns the linear transformation that
    /// transforms a reference cube into the provided Box.
    Eigen::AlignedScaling3f box_reference_transform(const Box & box)
    {
        return Scaling(box.extents);
    }

    Eigen::AlignedScaling3f rectangle_reference_transform(const Rectangle & rectangle)
    {
        const auto & extents = rectangle.extents;
        return Scaling(extents.x(), extents.y(), 1.0f);
    }

    Eigen::AlignedScaling3f sphere_reference_transform(const Sphere & sphere)
    {
        const auto r = sphere.radius;
        return Scaling(r, r, r);
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

        // Set up uniforms that are invariant across renderables
        mesh_shader.use();
        mesh_shader.set_light_color(light_color);
        mesh_shader.set_light_direction(light_dir);
        mesh_shader.set_view_transform(view);
        mesh_shader.set_projection_transform(projection);
        mesh_shader.set_camera_position(camera.position());
        line_shader.use();
        line_shader.set_projection_transform(projection);
        line_shader.set_view_transform(view);

        // Face culling is necessary to properly render rectangles from both sides
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        render_primitives(buffer.rectangles(), shaders, gl_rectangle, rectangle_reference_transform);

        // But we don't want to cull faces for other objects (for now, at least)
        glDisable(GL_CULL_FACE);
        render_primitives(buffer.boxes(), shaders, gl_cube, box_reference_transform);
        render_primitives(buffer.spheres(), shaders, gl_sphere, sphere_reference_transform);
    }
}
