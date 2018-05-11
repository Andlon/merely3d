#pragma once

#include <merely3d/unused.hpp>

namespace merely3d
{
    class Window;
    class Frame;

    enum class MouseButton
    {
        Left,
        Right,
        Middle,
        Other
    };

    enum class Key
    {
        Unknown,
        Space,
        Apostrophe,
        Comma,
        Minus,
        Period,
        Slash,
        Num0,
        Num1,
        Num2,
        Num3,
        Num4,
        Num5,
        Num6,
        Num7,
        Num8,
        Num9,
        Semicolon,
        Equal,
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
        LeftBracket,
        Backslash,
        RightBracket,
        GraveAccent,
        World1,
        World2,
        Escape,
        Enter,
        Tab,
        Backspace,
        Insert,
        Delete,
        Right,
        Left,
        Down,
        Up,
        PageUp,
        PageDown,
        Home,
        End,
        CapsLock,
        ScrollLock,
        NumLock,
        PrintScreen,
        Pause,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,
        F14,
        F15,
        F16,
        F17,
        F18,
        F19,
        F20,
        F21,
        F22,
        F23,
        F24,
        F25,
        Kp0,
        Kp1,
        Kp2,
        Kp3,
        Kp4,
        Kp5,
        Kp6,
        Kp7,
        Kp8,
        Kp9,
        KpDecimal,
        KpDivide,
        KpMultiply,
        KpSubtract,
        KpAdd,
        KpEnter,
        KpEqual,
        LeftShift,
        LeftControl,
        LeftAlt,
        LeftSuper,
        RightShift,
        RightControl,
        RightAlt,
        RightSuper,
        Menu
    };

    /// Bitflags used to determine which modifiers are active during
    /// a key/mouse button press.
    enum Modifier
    {
        Shift = 0x0001,
        Control = 0x0002,
        Alt = 0x0004,
        Super = 0x0008
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
            MERELY_UNUSED(window);
            MERELY_UNUSED(key);
            MERELY_UNUSED(action);
            MERELY_UNUSED(scancode);
            MERELY_UNUSED(modifiers);
            return false;
        }

        virtual bool mouse_button_press(Window & window,
                                        MouseButton button,
                                        Action action,
                                        int modifiers)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(button);
            MERELY_UNUSED(action);
            MERELY_UNUSED(modifiers);
            return false;
        }

        virtual bool mouse_move(Window & window, double xpos, double ypos)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(xpos);
            MERELY_UNUSED(ypos);
            return false;
        }

        virtual void mouse_enter(Window & window)
        {
            MERELY_UNUSED(window);
        }

        virtual void mouse_leave(Window & window)
        {
            MERELY_UNUSED(window);
        }

        virtual bool scroll(Window & window, double xoffset, double yoffset)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(xoffset);
            MERELY_UNUSED(yoffset);
            return false;
        }

        virtual bool character_input(Window & window,
                                     unsigned int codepoint)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(codepoint);
            return false;
        }

        virtual void before_frame(Window & window, Frame & frame)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(frame);
        }

        virtual void after_frame(Window & window, double frame_duration)
        {
            MERELY_UNUSED(window);
            MERELY_UNUSED(frame_duration);
        }
    };
}
