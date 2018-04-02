#include <merely3d/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.hpp"

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

typedef void(*GlfwWindowDestroyFunc)(GLFWwindow *);
typedef std::unique_ptr<GLFWwindow, GlfwWindowDestroyFunc> GlfwWindowPtr;

namespace
{
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
}

namespace merely3d
{
    class Window::WindowData
    {
    public:
        WindowData(GlfwWindowPtr ptr, ShaderProgram program)
            : glfw_window(std::move(ptr)),
              default_program(std::move(program)),
              viewport_size(0, 0),
              rectangle_vbo(0),
              rectangle_vao(0),
              rectangle_ebo(0) {}

        GlfwWindowPtr glfw_window;
        std::pair<int, int> viewport_size;

        ShaderProgram default_program;

        GLuint rectangle_vbo;
        GLuint rectangle_vao;
        GLuint rectangle_ebo;
    };

    static void check_and_update_viewport_size(GLFWwindow * window, int & viewport_width, int & viewport_height)
    {
        int fb_width;
        int fb_height;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);

        if (fb_width != viewport_width || fb_height != viewport_height)
        {
            viewport_width = fb_width;
            viewport_height = fb_height;
            glViewport(0, 0, viewport_width, viewport_height);
        }
    }

    Window::Window(Window && other)
        : _d(nullptr)
    {
        if (other._d)
        {
            _d = other._d;
            other._d = nullptr;
        }
    }

    Window::~Window()
    {
        // Must check for valid data because data might have been moved
        if (_d)
        {
            // TODO: Destroy ALL vertex buffers/objects and so forth
            glfwMakeContextCurrent(_d->glfw_window.get());
            glDeleteVertexArrays(1, &_d->rectangle_vao);
            glDeleteBuffers(1, &_d->rectangle_vbo);
            glDeleteBuffers(1, &_d->rectangle_ebo);
            delete _d;
        }
    }

    Window::Window(WindowData * data)
    {
        assert(data);
        _d = data;
    }

    void Window::render_frame_impl(Frame & frame)
    {
        assert(_d);
        glfwMakeContextCurrent(_d->glfw_window.get());

        check_and_update_viewport_size(_d->glfw_window.get(),
                                       _d->viewport_size.first,
                                       _d->viewport_size.second);

        // TODO: Make clear color configurable
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Process resulting command buffer from Frame
        auto & program = _d->default_program;

        program.use();

        // TODO: this should be done once, at the time when the
        // program is created
        const auto projection_loc = program.get_uniform_loc("projection");
        const auto modelview_loc = program.get_uniform_loc("modelview");

        const auto viewport_width = static_cast<float>(_d->viewport_size.first);
        const auto viewport_height = static_cast<float>(_d->viewport_size.second);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = viewport_width > 0.0 && viewport_height > 0.0
                                    ? viewport_width / viewport_height
                                    : 1.0;
        const auto projection = projection_matrix(0.785398, aspect_ratio, 0.1);

        glBindVertexArray(_d->rectangle_vao);

        for (const auto & rectangle : frame._rectangles)
        {
            const Eigen::Affine3f & modelview = rectangle.transform;
            program.set_mat4_uniform(projection_loc, projection.data());
            program.set_mat4_uniform(modelview_loc, modelview.data());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(_d->glfw_window.get());

        /* Poll for and process events */
        glfwPollEvents();
    }

    bool Window::should_close() const
    {
        return glfwWindowShouldClose(_d->glfw_window.get());
    }

    Window WindowBuilder::build() const
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // This is apparently needed on Mac OS X. Can we simply set it for all platforms...?
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        GLFWwindow * glfw_window = glfwCreateWindow(_width, _height, _title.c_str(), NULL, NULL);

        if (!glfw_window)
        {
            // TODO: Better error, more information
            throw std::runtime_error("Failed to initialize GLFWwindow");
        }

        // TODO: Should glad initialization happen in a different location...?
        // Is it safe to re-initialize GLAD?
        glfwMakeContextCurrent(glfw_window);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
        {
            // TODO: Better error message
            throw std::runtime_error("Failed to initialize GLAD");
        }

        const auto fragment_shader = Shader::compile(ShaderType::Fragment, DEFAULT_FSHADER);
        const auto vertex_shader = Shader::compile(ShaderType::Vertex, DEFAULT_VSHADER);
        auto default_program = ShaderProgram::create();
        default_program.attach(fragment_shader);
        default_program.attach(vertex_shader);
        default_program.link();

        // Construct VBO and VAO for the unit rectangle
        const auto rect_verts = unit_rectangle_vertices();
        const auto rect_idx = unit_rectangle_indices();

        // TODO: Encapsulate state of primitives in a separate class
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

        auto window_ptr = GlfwWindowPtr(glfw_window, glfwDestroyWindow);
        auto window_data = new Window::WindowData(std::move(window_ptr), std::move(default_program));
        window_data->rectangle_vao = rect_vao;
        window_data->rectangle_vbo = rect_vbo;
        auto window = Window(window_data);
        return std::move(window);
    }

}