#include "gl_gc.hpp"

namespace merely3d
{

void GlGarbagePile::delete_vertex_array_later(GLuint vao)
{
    _vao.push_back(vao);
}

void GlGarbagePile::delete_vertex_buffer_later(GLuint vbo)
{
    _vbo.push_back(vbo);
}

void GlGarbagePile::delete_element_buffer_later(GLuint ebo)
{
    _ebo.push_back(ebo);
}

void GlGarbageCollector::collect_garbage()
{
    auto & garbage = *_garbage;

    glDeleteBuffers(garbage._ebo.size(), garbage._ebo.data());
    glDeleteBuffers(garbage._vbo.size(), garbage._vbo.data());
    glDeleteVertexArrays(garbage._vao.size(), garbage._vao.data());
    garbage._ebo.clear();
    garbage._vbo.clear();
    garbage._vao.clear();
}

std::shared_ptr<GlGarbagePile> GlGarbageCollector::garbage() const
{
    return _garbage;
}

GlGarbageCollector::GlGarbageCollector()
    : _garbage(std::make_shared<GlGarbagePile>())
{ }

}
