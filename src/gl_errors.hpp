#pragma once

namespace merely3d
{
    namespace detail
    {
        void check_gl_errors(const char * file, int line);
    }
}

#define MERELY_CHECK_GL_ERRORS() ::merely3d::detail::check_gl_errors(__FILE__, __LINE__);