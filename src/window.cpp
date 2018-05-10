#include <merely3d/window.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <chrono>

#include "shader.hpp"
#include "command_buffer.hpp"
#include "renderer.hpp"
#include "event_convert.hpp"

typedef void(*GlfwWindowDestroyFunc)(GLFWwindow *);
typedef std::unique_ptr<GLFWwindow, GlfwWindowDestroyFunc> GlfwWindowPtr;

namespace merely3d
{
    const double NEAR_PLANE = 0.1f;

    /// Computes the "infinite" projection matrix with the given
    /// vertical field of view `fovy`, aspect ratio and the distance to
    /// the near plane.
    ///
    /// The projection matrix maps points from the view space in which
    /// negative z is "in front of" the camera. In other words,
    /// positive z values are not shown.
    template <typename T>
    Eigen::Matrix<T, 4, 4> projection_matrix(T fovy, T aspect_ratio, T near_plane_dist)
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
        Eigen::Matrix<T, 4, 4> p;
        p << (fovy / aspect_ratio),  0.0,  0.0,      0.0,
                            0.0, fovy,  0.0,      0.0,
                            0.0,  0.0, -1.0, -2.0 * n,
                            0.0,  0.0, -1.0,      0.0;
        return p;
    }

    template <typename T>
    Eigen::Matrix<T, 4, 4> projection_matrix(T fovy, T near_plane_dist, int viewport_width, int viewport_height)
    {
        const auto width = static_cast<double>(viewport_width);
        const auto height = static_cast<double>(viewport_height);

        // Guard against zero width/height, which may technically be a valid state
        const auto aspect_ratio = width > 0.0 && height > 0.0
                                  ? width / height
                                  : 1.0;
        return projection_matrix(fovy, static_cast<T>(aspect_ratio), near_plane_dist);
    }

    class Window::WindowData
    {
    public:
        WindowData(GlfwWindowPtr ptr, Renderer renderer)
            : glfw_window(std::move(ptr)),
              viewport_size(0, 0),
              renderer(std::move(renderer)),
              // Put the first frame time maximally far away in the future,
              // and special case when determining time since last frame
              previous_frame_time(std::chrono::steady_clock::time_point::max()),
              fovy(1.57)
        {}

        GlfwWindowPtr glfw_window;
        std::pair<int, int> viewport_size;

        Camera camera;

        CommandBuffer command_buffer;
        Renderer renderer;

        std::chrono::steady_clock::time_point previous_frame_time;
        std::vector<std::shared_ptr<EventHandler>> event_handlers;

        float fovy;
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

    void dispatch_key_event(Window * window,
                            Key key,
                            Action action,
                            int scancode,
                            int modifiers)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            const auto stop_propagate =
                    handler->key_press(*window, key, action, scancode, modifiers);
            if (stop_propagate)
            {
                break;
            }
        }
    }

    void dispatch_mouse_button_event(Window * window,
                                     MouseButton button,
                                     Action action,
                                     int modifiers)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            const auto stop_propagate = handler->mouse_button_press(*window, button, action, modifiers);
            if (stop_propagate)
            {
                break;
            }
        }
    }

    void dispatch_mouse_move_event(Window *window, double xpos, double ypos)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            const auto stop_propagate = handler->mouse_move(*window, xpos, ypos);
            if (stop_propagate)
            {
                break;
            }
        }
    }

    void dispatch_mouse_enter_event(Window *window, bool entered)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            if (entered)
            {
                handler->mouse_enter(*window);
            }
            else
            {
                handler->mouse_leave(*window);
            }
        }
    }

    void dispatch_character_input_event(Window *window, unsigned int codepoint)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            const auto stop_propagate = handler->character_input(*window, codepoint);
            if (stop_propagate)
            {
                break;
            }
        }
    }

    void dispatch_scroll_event(Window *window, double xoffset, double yoffset)
    {
        for (auto & handler : window->_d->event_handlers)
        {
            const auto stop_propagate = handler->scroll(*window, xoffset, yoffset);
            if (stop_propagate)
            {
                break;
            }
        }
    }

    static void key_callback(GLFWwindow * glfw_window,
                             int glfw_key,
                             int scancode,
                             int glfw_action,
                             int mods)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        const auto key = key_from_glfw(glfw_key);
        const auto action = action_from_glfw(glfw_action);
        dispatch_key_event(window_ptr, key, action, scancode, mods);
    }

    static void mouse_button_callback(GLFWwindow * glfw_window,
                                      int glfw_button,
                                      int glfw_action,
                                      int mods)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        const auto button = mouse_button_from_glfw(glfw_button);
        const auto action = action_from_glfw(glfw_action);
        dispatch_mouse_button_event(window_ptr, button, action, mods);
    }

    static void character_input_callback(GLFWwindow * glfw_window, unsigned int codepoint)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        dispatch_character_input_event(window_ptr, codepoint);
    }

    static void scroll_callback(GLFWwindow * glfw_window, double xoffset, double yoffset)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        dispatch_scroll_event(window_ptr, xoffset, yoffset);
    }

    static void cursor_pos_callback(GLFWwindow * glfw_window, double xpos, double ypos)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        dispatch_mouse_move_event(window_ptr, xpos, ypos);
    }

    static void cursor_enter_callback(GLFWwindow * glfw_window, int entered)
    {
        auto window_ptr = static_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
        dispatch_mouse_enter_event(window_ptr, static_cast<bool>(entered));
    }

    Window::Window(Window && other)
        : _d(nullptr)
    {
        if (other._d)
        {
            _d = other._d;
            other._d = nullptr;
            glfwSetWindowUserPointer(_d->glfw_window.get(), this);
        }
    }

    Window::~Window()
    {
        // Must check for valid data because data might have been moved
        if (_d)
        {
            // TODO: Destroy ALL vertex buffers/objects and so forth
            make_current();
            delete _d;
        }
    }

    Window::Window(WindowData * data)
    {
        assert(data);
        _d = data;
        glfwSetWindowUserPointer(data->glfw_window.get(), this);
    }

    void Window::render_frame_impl(Frame & frame)
    {
        MERELY_UNUSED(frame);
        assert(_d);

        auto & vp_width = _d->viewport_size.first;
        auto & vp_height = _d->viewport_size.second;

        check_and_update_viewport_size(_d->glfw_window.get(), vp_width, vp_height);
        const auto projection = projection_matrix(static_cast<double>(_d->fovy), NEAR_PLANE, vp_width, vp_height);

        _d->renderer.render(_d->command_buffer, _d->camera, projection.cast<float>());

        get_command_buffer()->clear();

        end_frame();

        glfwSwapBuffers(_d->glfw_window.get());
    }

    bool Window::should_close() const
    {
        return glfwWindowShouldClose(_d->glfw_window.get());
    }

    Camera & Window::camera()
    {
        return _d->camera;
    }

    const Camera & Window::camera() const
    {
        return _d->camera;
    }

    void Window::add_event_handler(std::shared_ptr<EventHandler> handler)
    {
        _d->event_handlers.push_back(std::move(handler));
    }

    Frame Window::begin_frame()
    {
        double time_since_previous_frame = 0.0;
        const auto now = std::chrono::steady_clock::now();
        if (_d->previous_frame_time < now)
        {
            const std::chrono::duration<double> duration = now - _d->previous_frame_time;
            time_since_previous_frame = duration.count();
        }
        _d->previous_frame_time = now;

        Frame frame(get_command_buffer(), time_since_previous_frame);
        for (auto & handler : _d->event_handlers)
        {
            handler->before_frame(*this, frame);
        }

        return std::move(frame);
    }

    void Window::end_frame()
    {
        const auto now = std::chrono::steady_clock::now();
        const std::chrono::duration<double> duration = now - _d->previous_frame_time;

        for (auto & handler : _d->event_handlers)
        {
            handler->after_frame(*this, duration.count());
        }
    }

    CommandBuffer * Window::get_command_buffer()
    {
        return &_d->command_buffer;
    }

    GLFWwindow *Window::glfw_window() {
        return _d->glfw_window.get();
    }

    void Window::make_current()
    {
        glfwMakeContextCurrent(_d->glfw_window.get());
    }

    void Window::poll_events()
    {
        glfwPollEvents();
    }

    void Window::set_cursor_mode(CursorMode mode)
    {
        auto glfw_window = _d->glfw_window.get();

        switch (mode)
        {
            case CursorMode::Normal: glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); break;
            case CursorMode::Hidden: glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); break;
            case CursorMode::Disabled: glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); break;
        }
    }

    Action Window::get_last_key_action(Key key) const
    {
        const auto glfw_action = glfwGetKey(_d->glfw_window.get(), glfw_key_from_key(key));
        return action_from_glfw(glfw_action);
    }

    ScreenCoords Window::get_current_cursor_position() const
    {
        ScreenCoords s;
        glfwGetCursorPos(_d->glfw_window.get(), &s.x, &s.y);
        return s;
    }

    Eigen::Vector3f Window::unproject_screen_coordinates(const ScreenCoords & coords)
    {
        const auto vp_width = _d->viewport_size.first;
        const auto vp_height = _d->viewport_size.second;

        const auto projection = projection_matrix(static_cast<double>(fovy()), NEAR_PLANE, vp_width, vp_height);

        /// Transform screen coordinates into normalized device coordinates
        const auto window_size = this->size();
        const auto w = static_cast<double>(window_size.width);
        const auto h = static_cast<double>(window_size.height);
        const auto ndc_x = 2.0 * (coords.x - w / 2.0) / w;
        const auto ndc_y = -2.0 * (coords.y - h / 2.0) / h;

        /// Define point in normalized device coordinates and project it back, using
        /// the assumption that it's located on the near plane.
        const Eigen::Vector4d ndc_point(ndc_x, ndc_y, -1.0, 1.0);
        const Eigen::Vector4d view_point = projection.inverse() * NEAR_PLANE * ndc_point;

        return view_point.head<3>().cast<float>();
    }

    WindowSize Window::size() const
    {
        WindowSize size;
        auto glfw_window = _d->glfw_window.get();
        glfwGetWindowSize(glfw_window, &size.width, &size.height);
        return size;
    }

    float Window::fovy() const
    {
        return _d->fovy;
    }

    void Window::set_fovy(float fovy)
    {
        if (fovy <= 0.0f || fovy >= 3.142f)
        {
            throw std::invalid_argument("fovy must be a positive number between 0 and PI");
        }
        _d->fovy = fovy;
    }

    Window WindowBuilder::build() const
    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // This is apparently needed on Mac OS X. Can we simply set it for all platforms...?
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        glfwWindowHint(GLFW_SAMPLES, _samples);

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

        glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
        glfwSetKeyCallback(glfw_window, key_callback);
        glfwSetCharCallback(glfw_window, character_input_callback);
        glfwSetScrollCallback(glfw_window, scroll_callback);
        glfwSetCursorPosCallback(glfw_window, cursor_pos_callback);
        glfwSetCursorEnterCallback(glfw_window, cursor_enter_callback);

        auto renderer = Renderer::build();
        auto window_ptr = GlfwWindowPtr(glfw_window, glfwDestroyWindow);
        auto window_data = new Window::WindowData(std::move(window_ptr), std::move(renderer));
        auto window = Window(window_data);
        return std::move(window);
    }

}
