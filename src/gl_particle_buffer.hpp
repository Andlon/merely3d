#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cassert>

#include <merely3d/primitives.hpp>
#include "gl_gc.hpp"
#include "gl_errors.hpp"

namespace merely3d
{

    class GlParticleBuffer
    {
    public:
        GlParticleBuffer(GlParticleBuffer && other) noexcept;
        ~GlParticleBuffer();

        GlParticleBuffer(const GlParticleBuffer & other) = delete;
        GlParticleBuffer & operator=(const GlParticleBuffer & other) = delete;
        GlParticleBuffer & operator=(GlParticleBuffer && other) = delete;

        /// Creates a new particle buffer.
        ///
        /// Note that the correct OpenGL context MUST be set prior to
        /// calling this function.
        static GlParticleBuffer create(const std::shared_ptr<GlGarbagePile> & garbage);

        /// Updates particle data on the GPU.
        ///
        /// Note that the correct OpenGL context MUST be set prior to
        /// calling this function.
        void update_buffer(const float * particles, size_t num_particles);

        void bind();

        void unbind();

    private:
        GlParticleBuffer(const std::shared_ptr<GlGarbagePile> & garbage, GLuint vao, GLuint vbo)
            : _vao(vao), _vbo(vbo), _garbage(garbage)
        {}

        GLuint _vao;
        GLuint _vbo;

        std::shared_ptr<GlGarbagePile> _garbage;
    };

    inline GlParticleBuffer::GlParticleBuffer(GlParticleBuffer && other) noexcept
        : _vao(other._vao),
          _vbo(other._vbo),
          _garbage(other._garbage)
    {
        other._garbage.reset();
    }

    inline GlParticleBuffer::~GlParticleBuffer()
    {
        if (_garbage)
        {
            _garbage->delete_vertex_buffer_later(_vbo);
            _garbage->delete_vertex_array_later(_vao);
        }
    }

    inline GlParticleBuffer GlParticleBuffer::create(const std::shared_ptr<GlGarbagePile> & garbage)
    {
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), NULL);
        glEnableVertexAttribArray(0);
        // color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // radius attribute
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        return { garbage, vao, vbo };
    }

    inline void GlParticleBuffer::bind()
    {
        glBindVertexArray(_vao);
    }

    inline void GlParticleBuffer::unbind()
    {
        glBindVertexArray(0);
    }

    inline void GlParticleBuffer::update_buffer(const float * particle_data, size_t num_particles)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        MERELY_CHECK_GL_ERRORS();

        GLint current_gpu_buffer_size = 0;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &current_gpu_buffer_size);
        MERELY_CHECK_GL_ERRORS();

        constexpr size_t NUM_FLOATS_PER_PARTICLES = 7;

        const auto buffer_size = static_cast<GLint>(sizeof(float) * NUM_FLOATS_PER_PARTICLES * num_particles);

        // TODO: Use geometric increments to avoid reallocating when a small number of
        // particles are added at each time step (which would then cause a full reallocation
        // on each time step).
        if (current_gpu_buffer_size < buffer_size)
        {
            glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STREAM_DRAW);
            MERELY_CHECK_GL_ERRORS();
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, static_cast<const void*>(particle_data));
        MERELY_CHECK_GL_ERRORS();

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}
