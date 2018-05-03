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

    TrianglePrimitiveRenderer TrianglePrimitiveRenderer::build()
    {
        const auto fragment_shader = Shader::compile(ShaderType::Fragment, shaders::default_fragment);
        const auto vertex_shader = Shader::compile(ShaderType::Vertex, shaders::default_vertex);
        auto default_program = ShaderProgram::create();
        default_program.attach(fragment_shader);
        default_program.attach(vertex_shader);
        default_program.link();

        const auto basic_fragment_shader = Shader::compile(ShaderType::Fragment, shaders::basic_fragment);
        const auto basic_vertex_shader = Shader::compile(ShaderType::Vertex, shaders::basic_vertex);
        auto basic_program = ShaderProgram::create();
        basic_program.attach(basic_fragment_shader);
        basic_program.attach(basic_vertex_shader);
        basic_program.link();

        const auto cube_verts = unit_cube_vertices_and_normals();
        auto gl_cube = GlPrimitive::create(cube_verts);

        const auto rect_verts = unit_rectangle_vertices_and_normals();
        auto gl_rect = GlPrimitive::create(rect_verts);

        const auto sphere_verts = unit_sphere_vertices_and_normals();
        auto gl_sphere = GlPrimitive::create(sphere_verts);

        return TrianglePrimitiveRenderer(std::move(default_program),
                                         std::move(basic_program),
                                         std::move(gl_cube),
                                         std::move(gl_rect),
                                         std::move(gl_sphere));
    }

    void TrianglePrimitiveRenderer::render(
                const CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection)
    {
        default_program.use();

        const auto projection_loc = default_program.get_uniform_loc("projection");
        const auto model_loc = default_program.get_uniform_loc("model");
        const auto view_loc = default_program.get_uniform_loc("view");
        const auto object_color_loc = default_program.get_uniform_loc("object_color");
        const auto light_color_loc = default_program.get_uniform_loc("light_color");
        const auto light_dir_loc = default_program.get_uniform_loc("light_dir");
        const auto normal_transform_loc = default_program.get_uniform_loc("normal_transform");
        const auto view_pos_loc = default_program.get_uniform_loc("view_pos");

        const auto basic_projection_loc = basic_shader_program.get_uniform_loc("projection");
        const auto basic_model_loc = basic_shader_program.get_uniform_loc("model");
        const auto basic_view_loc = basic_shader_program.get_uniform_loc("view");
        const auto basic_object_color_loc = basic_shader_program.get_uniform_loc("object_color");

        const Eigen::Affine3f view = camera.transform().inverse();

        // TODO: Make lighting configurable rather than hard-coded
        const auto light_color = Color(1.0, 1.0, 1.0);
        const auto light_color_array = light_color.into_array();
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        default_program.set_vec3_uniform(light_color_loc, light_color_array.data());
        default_program.set_vec3_uniform(light_dir_loc, light_dir.data());

        auto draw_primitive = [&] (const GlPrimitive & primitive,
                                   const Affine3f & model,
                                   const Material & material)
        {
            const auto normal_transform = Matrix3f(model.linear().inverse().transpose());
            const auto obj_color_array = material.color.into_array();

            // TODO: Rather than calling glPolygonMode on every render,
            // we should rather sort objects by properties so that we can
            // minimize state changes
            if (material.wireframe)
            {
                basic_shader_program.use();
                basic_shader_program.set_mat4_uniform(basic_projection_loc, projection.data());
                basic_shader_program.set_mat4_uniform(basic_view_loc, view.data());
                basic_shader_program.set_mat4_uniform(basic_model_loc, model.data());
                basic_shader_program.set_vec3_uniform(basic_object_color_loc, obj_color_array.data());
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                default_program.use();
                default_program.set_mat4_uniform(projection_loc, projection.data());
                default_program.set_mat4_uniform(view_loc, view.data());
                default_program.set_mat4_uniform(model_loc, model.data());
                default_program.set_mat3_uniform(normal_transform_loc, normal_transform.data());
                default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
                default_program.set_vec3_uniform(view_pos_loc, camera.position().data());
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }

            glDrawArrays(GL_TRIANGLES, 0, primitive.vertex_count());
        };

        gl_rectangle.bind();

        // Face culling is necessary to properly render rectangles from both sides
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        for (const auto & rectangle : buffer.rectangles())
        {
            const auto & extents = rectangle.shape.extents;
            const auto reference_transform = Scaling(extents.x(), extents.y(), 1.0f);
            const auto model = model_transform_from_reference(rectangle, reference_transform);
            draw_primitive(gl_rectangle, model, rectangle.material);
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
}
