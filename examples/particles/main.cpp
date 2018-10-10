#include <merely3d/app.hpp>
#include <merely3d/window.hpp>
#include <merely3d/camera_controller.hpp>

#include <Eigen/Geometry>

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Material;
using merely3d::Color;
using merely3d::Line;
using merely3d::EventHandler;
using merely3d::CameraController;

using merely3d::renderable;
using merely3d::Rectangle;
using merely3d::Box;
using merely3d::Sphere;
using merely3d::Particle;

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Quaternionf;
using Eigen::AngleAxisf;

int main()
{
    // Constructing the app first is essential: it makes sure that
    // GLFW is set up properly. Note that as an alternative, you can call
    // glfw init/terminate yourself directly, but you must be careful that
    // any windows are destroyed before calling terminate(). App automatically
    // takes care of this as long as it outlives any windows.
    merely3d::App app;

    {
        auto window = WindowBuilder()
                .dimensions(1024, 768)
                .title("Hello merely3d!")
                .multisampling(8)
                .build();

        window.camera().look_in(Vector3f(1.0, 0.0, -1), Vector3f(0.0, 0.0, 1.0));
        window.camera().set_position(Vector3f(-1.0, 0.0, 3.0));

        window.add_event_handler(std::shared_ptr<EventHandler>(new CameraController));

        const size_t num_particles_x = 10;
        const size_t num_particles_y = 10;
        const size_t num_particles_z = 10;

        while (!window.should_close())
        {
            window.render_frame([&] (Frame & frame)
            {
                for (size_t i = 0; i < num_particles_x; ++i)
                {
                    for (size_t j = 0; j < num_particles_y; ++j)
                    {
                        for (size_t k = 0; k < num_particles_z; ++k)
                        {
                            const auto x = static_cast<float>(i) / 2.0;
                            const auto y = static_cast<float>(j) / 2.0;
                            const auto z = static_cast<float>(k) / 2.0;
                            frame.draw_particle(Particle()
                                                        .with_position(Vector3f(x, y, z))
                                                        .with_radius(0.2)
                                                        .with_color(Color(105.0/255.0, 155.0/255.0, 234/255.0)));
                        }
                    }
                }

                frame.draw_line(Line(Eigen::Vector3f(0.0, 0.0, 0.0), Eigen::Vector3f(0.0, 0.0, 1.0)));
            });
        }
    }

    return 0;
}
