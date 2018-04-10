#pragma once

#include <merely3d/events.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <sstream>

namespace merely3d
{
    inline MouseButton mouse_button_from_glfw(int glfw_button)
    {
        switch (glfw_button)
        {
            case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
            case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
            case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
            default: return MouseButton::Other;
        }
    }

    // TODO: Move this to CPP file to avoid massive code bloat?
    // (unless the compiler is sufficiently smart to creating a separate function,
    // but this is perhaps unlikely to occur in debug mode at the very least)
    inline Key key_from_glfw(int glfw_key_code)
    {
        switch (glfw_key_code)
        {
            // TODO: More keys
            case GLFW_KEY_A: return Key::A;
            case GLFW_KEY_W: return Key::W;
            case GLFW_KEY_S: return Key::S;
            case GLFW_KEY_D: return Key::D;
            case GLFW_KEY_C: return Key::C;
            case GLFW_KEY_SPACE: return Key::Space;
            case GLFW_KEY_LEFT: return Key::Left;
            case GLFW_KEY_RIGHT: return Key::Right;
            case GLFW_KEY_DOWN: return Key::Down;
            case GLFW_KEY_UP: return Key::Up;
            default: return Key::Unknown;
        }
    }

    inline Action action_from_glfw(int glfw_action)
    {
        switch (glfw_action)
        {
            case GLFW_PRESS: return Action::Press;
            case GLFW_RELEASE: return Action::Release;
            case GLFW_REPEAT: return Action::Repeat;
            default:
                std::stringstream error;
                error << "Internal error: unhandled GLFW key action "
                      << glfw_action;
                throw std::runtime_error(error.str());
                break;
        }
    }

}
