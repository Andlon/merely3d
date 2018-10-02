#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_errors.hpp"

namespace {

    std::string gl_string_error(GLenum errorcode)
    {
        switch (errorcode)
        {
            case GL_NO_ERROR:
                return "No error.";
            case GL_INVALID_ENUM:
                return "Invalid enum.";
            case GL_INVALID_VALUE:
                return "Invalid value.";
            case GL_INVALID_OPERATION:
                return "Invalid operation.";
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                return "Invalid framebuffer operation.";
            case GL_OUT_OF_MEMORY:
                return "Out of memory.";
            case GL_STACK_UNDERFLOW:
                return "Stack underflow.";
            case GL_STACK_OVERFLOW:
                return "Stack overflow.";
            default:
                return "Unknown/unhandled error.";
        }
    }

}

namespace merely3d
{
    namespace detail
    {
        void check_gl_errors(const char * file, int line)
        {
            GLenum err = glGetError();
            if (err != GL_NO_ERROR)
            {
                std::cerr << "OpenGL errors in file \"" << file << "\", line " << line << ":" << std::endl;
                std::cerr << gl_string_error(err) << std::endl;

                while((err = glGetError()) != GL_NO_ERROR)
                {
                    std::cerr << "Error: " << gl_string_error(err) << std::endl;
                }
            }
        }
    }

}