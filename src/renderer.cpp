#include "renderer.hpp"
#include "gl_primitive.hpp"
#include "mesh_util.hpp"

#include <shaders.hpp>

using Eigen::Quaternionf;
using Eigen::Vector3f;
using Eigen::Affine3f;

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

    /// Returns the vertices of the unit rectangle,
    /// defined to lie in the xy-plane (z = 0),
    /// centered at (0, 0, 0) and with unit length sides.
    std::vector<float> unit_rectangle_vertices()
    {
        return {
             0.5, -0.5, 0.0,
             0.5,  0.5, 0.0,
            -0.5,  0.5, 0.0,
            -0.5, -0.5, 0.0
        };
    }

    /// Returns the vertices of the unit rectangle,
    /// defined to lie in the xy-plane (z = 0),
    /// centered at (0, 0, 0) and with unit length sides.
    std::vector<float> unit_rectangle_vertices_and_normals()
    {
        // We render both faces with opposite normals
        return {
             // Front
             0.5, -0.5, 0.0, 0.0, 0.0, 1.0,
             0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, 1.0,
            -0.5, -0.5, 0.0, 0.0, 0.0, 1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, 1.0,

             // Back
            -0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
             0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
             0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
            -0.5, -0.5, 0.0, 0.0, 0.0, -1.0,
            -0.5,  0.5, 0.0, 0.0, 0.0, -1.0,
        };
    }

    /// Returns the indices corresponding to the vertices
    /// obtained from unit_rectangle_vertices.
    std::vector<unsigned int> unit_rectangle_indices()
    {
        return {
             0, 1, 2,
             2, 3, 0
        };
    }

    std::vector<float> unit_cube_vertices()
    {
        const float s = 0.5;

        return {
            // Negative y
            -s, -s, s,  // 0, top-south-west
            -s, -s, -0.5, // 1, bottom-south-west
            s, -s, -s,  // 2, bottom-south-east
            s, -s, s,   // 3, top-south-east

            // Positive y
            -s, s, s,   // 4, top-north-west
            -s, s, -s,  // 5, bottom-north-west
            s, s, -s,   // 6, bottom-north-east
            s, s, s,    // 7, top-north-east
        };
    }

    /// Returns the vertices and normals of the unit rectangle,
    /// defined to lie in the xy-plane (z = 0),
    /// centered at (0, 0, 0) and with unit length sides.
    ///
    /// The data is organized follows:
    /// { v1_x, v1_y, v1_z, n1_x, n1_y, n1_z, v2_x, ... }
    /// where v1 is vertex 1, n1 is the normal of vertex 1 and so on.
    std::vector<float> unit_cube_vertices_and_normals()
    {
        // vertices and normals courtesy of learnopengl.com
        return {
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f
        };
    }


}

namespace merely3d
{
    Renderer::Renderer(ShaderProgram default_program,
                       ShaderProgram basic_shader_program,
                       GlPrimitive gl_cube,
                       GlPrimitive gl_rectangle,
                       GlPrimitive gl_sphere,
                       GlLine      gl_line)
        : default_program(std::move(default_program)),
          basic_shader_program(std::move(basic_shader_program)),
          gl_cube(std::move(gl_cube)),
          gl_rectangle(std::move(gl_rectangle)),
          gl_sphere(std::move(gl_sphere)),
          gl_line(std::move(gl_line))
    {

    }

    Renderer Renderer::build()
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

        auto gl_line = GlLine::create();

        return Renderer(std::move(default_program),
                        std::move(basic_program),
                        std::move(gl_cube),
                        std::move(gl_rect),
                        std::move(gl_sphere),
                        std::move(gl_line));
    }

    void Renderer::render(const CommandBuffer & buffer,
                          const Camera & camera,
                          int viewport_width,
                          int viewport_height)
    {
        // TODO: Make clear color configurable
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        default_program.use();

        // TODO: this should be done once, at the time when the
        // program is created
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

        // TODO: Move aspect ratio computation to separate function
        const auto width = static_cast<float>(viewport_width);
        const auto height = static_cast<float>(viewport_height);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = width > 0.0 && height > 0.0
                                  ? width / height
                                  : 1.0;
        const auto projection = projection_matrix(camera.fovy(), aspect_ratio, 0.1);
        const Eigen::Affine3f view = camera.transform().inverse();

        const auto light_color = Color(1.0, 1.0, 1.0);
        const auto light_color_array = light_color.into_array();

        // TODO: Make lighting configurable rather than hard-coded
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        // Face culling is necessary to properly render rectangles from both sides
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        default_program.set_vec3_uniform(light_color_loc, light_color_array.data());
        default_program.set_vec3_uniform(light_dir_loc, light_dir.data());

        gl_rectangle.bind();

        for (const auto & rectangle : buffer.rectangles())
        {
            // TODO: Additional scaling from renderable!!
            const auto & extents = rectangle.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents.x(), extents.y(), 1.0);
            const Eigen::Affine3f model = Eigen::Translation3f(rectangle.position) *  rectangle.orientation * scaling;
            const auto normal_transform = Eigen::Matrix3f(model.linear().inverse().transpose());

            const auto obj_color_array = rectangle.material.color.into_array();
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(view_loc, view.data());
            default_program.set_mat4_uniform(model_loc, model.data());
            default_program.set_mat3_uniform(normal_transform_loc, normal_transform.data());
            default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
            default_program.set_vec3_uniform(view_pos_loc, camera.position().data());
            glDrawArrays(GL_TRIANGLES, 0, gl_rectangle.vertex_count());
        }

        gl_cube.bind();

        for (const auto & box : buffer.boxes())
        {
            // TODO: Additional scaling from renderable!!
            const auto & extents = box.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents);
            const Eigen::Affine3f model = Eigen::Translation3f(box.position) * box.orientation * scaling;
            const auto normal_transform = Eigen::Matrix3f(model.linear().inverse().transpose());

            const auto obj_color_array = box.material.color.into_array();
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(view_loc, view.data());
            default_program.set_mat4_uniform(model_loc, model.data());
            default_program.set_mat3_uniform(normal_transform_loc, normal_transform.data());
            default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
            default_program.set_vec3_uniform(view_pos_loc, camera.position().data());
            glDrawArrays(GL_TRIANGLES, 0, gl_cube.vertex_count());
        }

        gl_sphere.bind();

        for (const auto & sphere : buffer.spheres())
        {
            // TODO: Additional scaling from renderable!!
            const auto scaling = Eigen::Scaling(sphere.shape.radius);
            const Eigen::Affine3f model = Eigen::Translation3f(sphere.position) * sphere.orientation * scaling;
            const auto normal_transform = Eigen::Matrix3f(model.linear().inverse().transpose());

            const auto obj_color_array = sphere.material.color.into_array();
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(view_loc, view.data());
            default_program.set_mat4_uniform(model_loc, model.data());
            default_program.set_mat3_uniform(normal_transform_loc, normal_transform.data());
            default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
            default_program.set_vec3_uniform(view_pos_loc, camera.position().data());
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(gl_sphere.vertex_count()));
        }

        gl_line.bind();

        basic_shader_program.use();

        const Vector3f e1 = Vector3f::UnitX();

        for (const auto & line : buffer.lines())
        {
            const Eigen::Vector3f c = line.to - line.from;

            // The reference line is defined to be e1, i.e. [1, 0, 0], so we need the
            // rotation which rotates e1 into the direction of c.
            const Quaternionf rotation = Quaternionf::FromTwoVectors(e1, c);
            const Eigen::Affine3f model = Eigen::Translation3f(line.from) * rotation * Eigen::Scaling(c.norm());

            const auto obj_color_array = line.color.into_array();

            basic_shader_program.set_mat4_uniform(basic_projection_loc, projection.data());
            basic_shader_program.set_mat4_uniform(basic_view_loc, view.data());
            basic_shader_program.set_mat4_uniform(basic_model_loc, model.data());
            basic_shader_program.set_vec3_uniform(basic_object_color_loc, obj_color_array.data());
            glDrawArrays(GL_LINES, 0, 2);
        }

    }
}
