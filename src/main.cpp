#include <GLFW/glfw3.h>

#include <memory>
#include <iostream>

#include <merely3d/window.hpp>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Key;
using merely3d::KeyAction;
using merely3d::KeyListener;

using merely3d::Position;
using merely3d::Orientation;
using merely3d::Vector3;
using merely3d::Vector2;

using Eigen::Vector3f;

struct TestKeyListener : public merely3d::KeyListener
{
    virtual bool key_press(Window & window,
                           Key key,
                           KeyAction action,
                           int scancode,
                           int modifiers) override
    {
        if (key == Key::A)
        {
            std::cout << "Got A!" << std::endl;
        }
        else
        {
            std::cout << "Got something else." << std::endl;
        }
    }
};

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Scope is necessary because Window calls GL destructors in constructor
    {
        auto window = WindowBuilder()
                    .dimensions(1024, 768)
                    .title("Hello merely3d!")
                    .build();

        window.camera().look_in(Vector3f(1.0, 0.0, -1), Vector3f(0.0, 0.0, 1.0));
        window.camera().set_position(Vector3f(-1.0, 0.0, 3.0));

        window.add_key_listener(std::shared_ptr<KeyListener>(new TestKeyListener()));

        while (!window.should_close())
        {
            window.render_frame([] (Frame & frame)
            {
                Orientation orientation = Orientation::Identity();
                orientation = Eigen::AngleAxisf(0.78, Eigen::Vector3f(1.0, 0.0, 0.0));
                frame.draw_rectangle(Vector2(0.5, 0.5), Position(1.0, 0.0, 0.05), orientation);
                frame.draw_box(Vector3(1.0, 1.0, 1.0), Position(4.0, 0.0, 0.1));
            });
        }
    }

    glfwTerminate();
    return 0;
}
