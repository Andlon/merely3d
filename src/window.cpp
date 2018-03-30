#include <merely3d/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.hpp"

const std::string DEFAULT_VSHADER =
"#version 330 core                                       \n"
"layout (location = 0) in vec3 aPos;                     \n"
"                                                        \n"
"void main()                                             \n"
"{                                                       \n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);    \n"
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

        check_and_update_viewport_size(_d->glfw_window.get(), _d->viewport_size.first, _d->viewport_size.second);

        // TODO: Make clear color configurable
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Process resulting command buffer from Frame

        _d->default_program.use();

        for (const auto & rectangle : frame._rectangles)
        {
            glBindVertexArray(_d->rectangle_vao);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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