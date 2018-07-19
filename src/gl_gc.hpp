#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <memory>

namespace merely3d
{
    class GlGarbagePile
    {
    public:
        void delete_vertex_array_later(GLuint vao);
        void delete_vertex_buffer_later(GLuint vbo);
        void delete_element_buffer_later(GLuint ebo);

        // TODO: Delete programs/shaders

    private:
        std::vector<GLuint> _vao;
        std::vector<GLuint> _vbo;
        std::vector<GLuint> _ebo;

        friend class GlGarbageCollector;
    };

    class GlGarbageCollector
    {
    public:
        GlGarbageCollector();
        GlGarbageCollector(GlGarbageCollector &&) noexcept = default;
        GlGarbageCollector(const GlGarbageCollector &) = delete;

        /// Must only be called when the context is current!
        void collect_garbage();

        std::shared_ptr<GlGarbagePile> garbage() const;

    private:
        std::shared_ptr<GlGarbagePile> _garbage;
    };
}
