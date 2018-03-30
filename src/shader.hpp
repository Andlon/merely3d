#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace merely3d
{
    enum class ShaderType
    {
        Vertex,
        Fragment
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

    private:
        ShaderProgram() : _id(0) {}

        GLuint _id;
    };
}