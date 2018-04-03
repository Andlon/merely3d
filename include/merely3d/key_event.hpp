#pragma once

#include <merely3d/window.hpp>

namespace merely3d
{
    class Window;

    enum class Key
    {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Unknown
    };

    // TODO: This is not just for keys, also for mouse buttons, so rename to reflect!
    enum class KeyAction
    {
        Press,
        Repeat,
        Release
    };

    class KeyListener
    {
    public:
        virtual ~KeyListener() {};

        /// This function is called when the listener is notified about
        /// a key event. Returning true stops propagation of the event
        /// to other registered listeners, while returning false
        /// allows propagation to continue.
        // TODO: Introduce enum (class? Does not work well for bitflags, or?) for modifiers
        virtual bool key_press(Window & window,
                               Key key,
                               KeyAction action,
                               int scancode,
                               int modifiers) = 0;
    };
}
