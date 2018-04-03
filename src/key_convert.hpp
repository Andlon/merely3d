#pragma once

#include <merely3d/key_event.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <sstream>

namespace merely3d
{
    // TODO: Move this to CPP file to avoid massive code bloat?
    // (unless the compiler is sufficiently smart to creating a separate function,
    // but this is perhaps unlikely to occur in debug mode at the very least)
    inline Key key_from_glfw(int glfw_key_code)
    {
        switch (glfw_key_code)
        {
            // TODO: More keys
            case GLFW_KEY_A: return Key::A;
            default: return Key::Unknown;
        }
    }

    inline KeyAction action_from_glfw(int glfw_action)
    {
        switch (glfw_action)
        {
            case GLFW_PRESS: return KeyAction::Press;
            case GLFW_RELEASE: return KeyAction::Release;
            case GLFW_REPEAT: return KeyAction::Repeat;
            default:
                std::stringstream error;
                error << "Internal error: unhandled GLFW key action "
                      << glfw_action;
                throw std::runtime_error(error.str());
                break;
        }
    }

}
