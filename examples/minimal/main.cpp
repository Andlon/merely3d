#include <merely3d/app.hpp>
#include <merely3d/window.hpp>

#include <Eigen/Geometry>

#include "../demo/example_model.hpp"

using merely3d::Window;
using merely3d::WindowBuilder;
using merely3d::Frame;
using merely3d::Material;
using merely3d::Color;
using merely3d::Line;

using merely3d::renderable;
using merely3d::Rectangle;
using merely3d::Box;
using merely3d::Sphere;

using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Quaternionf;
using Eigen::AngleAxisf;

merely3d::StaticMesh load_example_model()
{
    // Create a static mesh which we can use when rendering
    const auto & vn = merely3d::example_model::vertices_and_normals;
    const auto & idx = merely3d::example_model::indices;
    const auto model_vertices_normals = std::vector<float>(vn.begin(), vn.end());
    const auto model_indices = std::vector<unsigned int>(idx.begin(), idx.end());
    return merely3d::StaticMesh(model_vertices_normals, model_indices);
}

/// This relatively minimal example is used mainly to test parts of merely3d
/// for memory errors or undefined behavior with sanitizers such as
/// -fsanitize=address and/or -fsanitize=undefined
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

        for (int i = 0; i < 100 && !window.should_close(); ++i)
        {
            window.render_frame([] (Frame & frame)
            {
                frame.draw(renderable(Rectangle(0.5, 0.5))
                            .with_position(1.0, 0.0, 0.5)
                            .with_orientation(AngleAxisf(0.78, Vector3f(1.0f, 0.0f, 0.0f)))
                            .with_material(Material().with_color(Color(0.5, 0.3, 0.3))));

                frame.draw_line(Line(Vector3f(0.0, 0.0, 0.0), Vector3f(10.0, -5.0, 10.0)));
                frame.draw(renderable(load_example_model()));
            });
        }
    }

    return 0;
}
