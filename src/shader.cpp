#include "shader.hpp"

#include <stdexcept>

namespace merely3d
{
    static GLuint convert_to_gl_shader_type(ShaderType type)
    {
        switch (type)
        {
            case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderType::Vertex: return GL_VERTEX_SHADER;
            case ShaderType::Geometry: return GL_GEOMETRY_SHADER;
            default: throw std::runtime_error("Unknown shader type");
        }
    }

    Shader Shader::compile(ShaderType type, const std::string & source)
    {
        const auto gl_fragment_type = convert_to_gl_shader_type(type);
        const auto shader_id = glCreateShader(gl_fragment_type);

        const auto source_c_str = source.c_str();

        glShaderSource(shader_id, 1, &source_c_str, NULL);
        glCompileShader(shader_id);

        int  success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char info_log[512];
            glGetShaderInfoLog(shader_id, 512, NULL, info_log);
            std::string error_msg(info_log);
            throw std::runtime_error("Shader compilation error: " + error_msg);
        }

        Shader shader;
        shader._id = shader_id;
        return shader;
    }

    ShaderProgram ShaderProgram::create()
    {
        ShaderProgram program;
        program._id = glCreateProgram();
        return program;
    }

    void ShaderProgram::attach(const Shader & shader)
    {
        glAttachShader(_id, shader._id);
    }

    void ShaderProgram::link()
    {
        glLinkProgram(_id);

        int success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char info_log[512];
            glGetProgramInfoLog(_id, 512, NULL, info_log);
            std::string error_msg(info_log);
            throw std::runtime_error("Program link error: " + error_msg);
        }
    }

    void ShaderProgram::use()
    {
        glUseProgram(_id);
    }

    GLint ShaderProgram::get_uniform_loc(const std::string & name) const
    {
        return glGetUniformLocation(_id, name.c_str());
    }

    void ShaderProgram::set_mat3_uniform(GLint location, const float * value)
    {
        glUniformMatrix3fv(location, 1, false, value);
    }

    void ShaderProgram::set_mat4_uniform(GLint location, const float * value)
    {
        glUniformMatrix4fv(location, 1, false, value);
    }

    void ShaderProgram::set_vec3_uniform(GLint location, const float * value)
    {
        glUniform3fv(location, 1, value);
    }

    void ShaderProgram::set_float_uniform(GLint location, float value)
    {
        glUniform1f(location, value);
    }
}
