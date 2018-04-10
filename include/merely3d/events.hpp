#pragma once

namespace merely3d
{
    class Window;

    enum class MouseButton
    {
        Left,
        Right,
        Middle,
        Other
    };

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
        Space,
        Left,
        Right,
        Up,
        Down,
        Unknown
    };

    // TODO: This is not just for keys, also for mouse buttons, so rename to reflect!
    enum class Action
    {
        Press,
        Repeat,
        Release
    };

    class EventHandler
    {
    public:
        virtual ~EventHandler() {};

        /// This function is called when the listener is notified about
        /// a key event. Returning true stops propagation of the event
        /// to other registered listeners, while returning false
        /// allows propagation to continue.
        // TODO: Introduce enum (class? Does not work well for bitflags, or?) for modifiers
        virtual bool key_press(Window & window,
                               Key key,
                               Action action,
                               int scancode,
                               int modifiers)
        {
            return false;
        }

        virtual bool mouse_button_press(Window & window,
                                        MouseButton button,
                                        Action action,
                                        int modifiers)
        {
            return false;
        }

        virtual void before_frame(Window & window, double time_since_previous_frame_begin) {}
        virtual void after_frame(Window & window, double frame_duration) {}
    };
}
