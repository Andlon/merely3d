#include "renderer.hpp"

#include <shaders.hpp>

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
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

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

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };
    }

    std::vector<unsigned int> unit_cube_indices()
    {
        return {
            // Southern face
            0, 1, 2,
            2, 3, 0,

            // Eastern face
            3, 2, 7,
            2, 6, 7,

            // Northern face
            5, 4, 7,
            7, 6, 5,

            // Western face
            1, 0, 4,
            4, 5, 1,

            // Bottom face
            6, 2, 1,
            6, 1, 5,

            // Top face
            0, 3, 7,
            7, 4, 0
        };
    }
}

namespace merely3d
{
    Renderer::Renderer(ShaderProgram program)
        : default_program(std::move(program))
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

        // Construct VBO and VAO for the unit rectangle
        // TODO: Add normals
        const auto rect_verts = unit_rectangle_vertices();
        const auto rect_idx = unit_rectangle_indices();

        // TODO: Encapsulate all this stuff in a StaticMesh type or similar
        GLuint rect_vao, rect_vbo, rect_ebo;
        glGenVertexArrays(1, &rect_vao);
        glBindVertexArray(rect_vao);
        glGenBuffers(1, &rect_vbo);
        glGenBuffers(1, &rect_ebo);
        glBindBuffer(GL_ARRAY_BUFFER, rect_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect_ebo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * rect_verts.size(), rect_verts.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * rect_idx.size(), rect_idx.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        // Construct VBO and VAO for the unit cube
        const auto cube_verts = unit_cube_vertices_and_normals();

        GLuint cube_vao, cube_vbo;
        glGenVertexArrays(1, &cube_vao);
        glBindVertexArray(cube_vao);
        glGenBuffers(1, &cube_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_verts.size(), cube_verts.data(), GL_STATIC_DRAW);
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        Renderer renderer(std::move(default_program));
        renderer.rectangle_ebo = rect_ebo;
        renderer.rectangle_vbo = rect_vbo;
        renderer.rectangle_vao = rect_vao;
        renderer.cube_vbo = cube_vbo;
        renderer.cube_vao = cube_vao;
        return std::move(renderer);
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
        const auto modelview_loc = default_program.get_uniform_loc("modelview");
        const auto object_color_loc = default_program.get_uniform_loc("object_color");
        const auto light_color_loc = default_program.get_uniform_loc("light_color");
        const auto light_dir_loc = default_program.get_uniform_loc("light_dir");
        const auto normal_transform_loc = default_program.get_uniform_loc("normal_transform");

        // TODO: Move aspect ratio computation to separate function
        const auto width = static_cast<float>(viewport_width);
        const auto height = static_cast<float>(viewport_height);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = viewport_width > 0.0 && viewport_height > 0.0
                                    ? viewport_width / viewport_height
                                    : 1.0;
        const auto projection = projection_matrix(1.57, aspect_ratio, 0.1);
        const Eigen::Affine3f view = camera.transform().inverse();

        const auto light_color = Color(1.0, 1.0, 1.0);
        const auto light_color_array = light_color.into_array();

        // TODO: Make lighting configurable rather than hard-coded
        const Eigen::Vector3f light_dir = Eigen::Vector3f(0.9, 1.2, -0.8).normalized();

        glBindVertexArray(rectangle_vao);

        default_program.set_vec3_uniform(light_color_loc, light_color_array.data());
        default_program.set_vec3_uniform(light_dir_loc, light_dir.data());

        for (const auto & rectangle : buffer.rectangles())
        {
            const auto & extents = rectangle.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents.x(), extents.y(), 0.0);
            const Eigen::Affine3f model_transform = Eigen::Translation3f(rectangle.position) *  rectangle.orientation * scaling;

            const Eigen::Affine3f modelview = view * model_transform;
            const auto obj_color_array = rectangle.material.color.into_array();
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(modelview_loc, modelview.data());
            default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }


        glBindVertexArray(cube_vao);

        default_program.set_vec3_uniform(light_color_loc, light_color_array.data());
        default_program.set_vec3_uniform(light_dir_loc, light_dir.data());

        for (const auto & box : buffer.boxes())
        {
            const auto & extents = box.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents);
            const Eigen::Affine3f model_transform = Eigen::Translation3f(box.position) * box.orientation * scaling;
            const Eigen::Affine3f modelview = view * model_transform;
            const auto normal_transform = Eigen::Matrix3f(model_transform.linear().inverse().transpose());

            const auto obj_color_array = box.material.color.into_array();
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(modelview_loc, modelview.data());
            default_program.set_mat3_uniform(normal_transform_loc, normal_transform.data());
            default_program.set_vec3_uniform(object_color_loc, obj_color_array.data());
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

    }
}
