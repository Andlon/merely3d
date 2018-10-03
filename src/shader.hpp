#pragma once

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace merely3d
{
    enum class ShaderType
    {
        Vertex,
        Fragment,
        Geometry
    };

    // TODO: Cleanup shaders when they are not needed anymore, somehow?
    class Shader
    {
    public:
        /// Compiles the shader, provided as source code in a string.
        ///
        /// Note that the correct context *must* have been set before compiling the shader.
        static Shader compile(ShaderType type, const std::string & source);

    private:
        Shader() : _id(0) {}
        GLuint _id;
        friend class ShaderProgram;
    };

    // TODO: Cleanup programs when they are not needed anymore?
    class ShaderProgram
    {
    public:
        ShaderProgram(const ShaderProgram &) = delete;

        ShaderProgram(ShaderProgram &&) = default;

        /// Creates a new shader program. The context *must* have correctly been set beforehand.
        static ShaderProgram create();

        /// Attaches the given shader to this program. The context *must* have correctly been set beforehand.
        void attach(const Shader & shader);

        /// Links the current program. The context *must* have correctly been set beforehand.
        void link();

        /// Makes GL use the current program. The context *must* have correctly been set beforehand.
        void use();

        // TODO: This kinda breaks encapsulation, but this class needs a redesign in any case
        GLint get_uniform_loc(const std::string & name) const;

        void set_mat3_uniform(GLint location, const float * value);
        void set_mat4_uniform(GLint location, const float * value);
        void set_vec3_uniform(GLint location, const float * value);
        void set_float_uniform(GLint location, float value);

    private:
        ShaderProgram() : _id(0) {}

        GLuint _id;
    };
}
