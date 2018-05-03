#pragma once

#include "gl_line.hpp"
#include "gl_primitive.hpp"
#include "shader.hpp"
#include "command_buffer.hpp"

#include <merely3d/camera.hpp>

#include <vector>

namespace merely3d
{

class TrianglePrimitiveRenderer
{
public:
    void render(const CommandBuffer & buffer,
                const Camera & camera,
                const Eigen::Matrix4f & projection);

    static TrianglePrimitiveRenderer build();

private:

    TrianglePrimitiveRenderer(ShaderProgram && default_program,
                              ShaderProgram && basic_program,
                              GlPrimitive && gl_cube,
                              GlPrimitive && gl_rectangle,
                              GlPrimitive && gl_sphere)
        : default_program(std::move(default_program)),
          basic_shader_program(std::move(basic_program)),
          gl_cube(std::move(gl_cube)),
          gl_rectangle(std::move(gl_rectangle)),
          gl_sphere(std::move(gl_sphere))
    {}

    ShaderProgram default_program;
    ShaderProgram basic_shader_program;

    GlPrimitive gl_cube;
    GlPrimitive gl_rectangle;
    GlPrimitive gl_sphere;
};

}
