#include "renderer.hpp"

namespace
{
    const std::string DEFAULT_VSHADER =
    "#version 330 core                                       \n"
    "layout (location = 0) in vec3 aPos;                     \n"
    "                                                        \n"
    "uniform mat4 projection;                                \n"
    "uniform mat4 modelview;                                 \n"
    "                                                        \n"
    "void main()                                             \n"
    "{                                                       \n"
    "    vec4 model_pos = vec4(aPos, 1.0);                   \n"
    "    gl_Position = projection * modelview * model_pos;   \n"
    "}                                                       \n"
    ;

    const std::string DEFAULT_FSHADER=
    "#version 330 core                                       \n"
    "out vec4 FragColor;                                     \n"
    "                                                        \n"
    "void main()                                             \n"
    "{                                                       \n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);            \n"
    "}                                                       \n"
    ;

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
        const auto fragment_shader = Shader::compile(ShaderType::Fragment, DEFAULT_FSHADER);
        const auto vertex_shader = Shader::compile(ShaderType::Vertex, DEFAULT_VSHADER);
        auto default_program = ShaderProgram::create();
        default_program.attach(fragment_shader);
        default_program.attach(vertex_shader);
        default_program.link();

        // Construct VBO and VAO for the unit rectangle
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
        const auto cube_verts = unit_cube_vertices();
        const auto cube_idx = unit_cube_indices();

        GLuint cube_vao, cube_vbo, cube_ebo;
        glGenVertexArrays(1, &cube_vao);
        glBindVertexArray(cube_vao);
        glGenBuffers(1, &cube_vbo);
        glGenBuffers(1, &cube_ebo);
        glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cube_verts.size(), cube_verts.data(), GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * cube_idx.size(), cube_idx.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        Renderer renderer(std::move(default_program));
        renderer.rectangle_ebo = rect_ebo;
        renderer.rectangle_vbo = rect_vbo;
        renderer.rectangle_vao = rect_vao;
        renderer.cube_ebo = cube_ebo;
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

        // TODO: Move aspect ratio computation to separate function
        const auto width = static_cast<float>(viewport_width);
        const auto height = static_cast<float>(viewport_height);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = viewport_width > 0.0 && viewport_height > 0.0
                                    ? viewport_width / viewport_height
                                    : 1.0;
        const auto projection = projection_matrix(1.57, aspect_ratio, 0.1);
        const Eigen::Affine3f view = camera.transform().inverse();

        glBindVertexArray(rectangle_vao);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        for (const auto & rectangle : buffer.rectangles())
        {
            const auto & extents = rectangle.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents.x(), extents.y(), 0.0);
            const Eigen::Affine3f transform = Eigen::Translation3f(rectangle.position) *  rectangle.orientation * scaling;

            const Eigen::Affine3f modelview = view * transform;
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(modelview_loc, modelview.data());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        glBindVertexArray(cube_vao);

        for (const auto & box : buffer.boxes())
        {
            const auto & extents = box.shape.extents;
            const auto scaling = Eigen::DiagonalMatrix<float, 3>(extents);
            const Eigen::Affine3f transform = Eigen::Translation3f(box.position) * box.orientation * scaling;
            const Eigen::Affine3f modelview = view * transform;
            default_program.set_mat4_uniform(projection_loc, projection.data());
            default_program.set_mat4_uniform(modelview_loc, modelview.data());
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
        }

    }
}
