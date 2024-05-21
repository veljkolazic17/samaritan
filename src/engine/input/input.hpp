#pragma once
#include <defines.hpp>

#define DEFINE_KEY(name, code) KEY_##name = code

BEGIN_NAMESPACE

namespace Input
{
    enum class Key
    {
        DEFINE_KEY(BACKSPACE, 0x08),
        DEFINE_KEY(ENTER, 0x0D),
        DEFINE_KEY(TAB, 0x09),
        DEFINE_KEY(SHIFT, 0x10),
        DEFINE_KEY(CONTROL, 0x11),

        DEFINE_KEY(PAUSE, 0x13),
        DEFINE_KEY(CAPITAL, 0x14),

        DEFINE_KEY(ESCAPE, 0x1B),

        DEFINE_KEY(CONVERT, 0x1C),
        DEFINE_KEY(NONCONVERT, 0x1D),
        DEFINE_KEY(ACCEPT, 0x1E),
        DEFINE_KEY(MODECHANGE, 0x1F),

        DEFINE_KEY(SPACE, 0x20),
        DEFINE_KEY(PRIOR, 0x21),
        DEFINE_KEY(NEXT, 0x22),
        DEFINE_KEY(END, 0x23),
        DEFINE_KEY(HOME, 0x24),
        DEFINE_KEY(LEFT, 0x25),
        DEFINE_KEY(UP, 0x26),
        DEFINE_KEY(RIGHT, 0x27),
        DEFINE_KEY(DOWN, 0x28),
        DEFINE_KEY(SELECT, 0x29),
        DEFINE_KEY(PRINT, 0x2A),
        //Don't change name of key
        DEFINE_KEY(EXEC, 0x2B),
        DEFINE_KEY(SNAPSHOT, 0x2C),
        DEFINE_KEY(INSERT, 0x2D),
        //Don't change name of key
        DEFINE_KEY(DEL, 0x2E),
        DEFINE_KEY(HELP, 0x2F),

        DEFINE_KEY(A, 0x41),
        DEFINE_KEY(B, 0x42),
        DEFINE_KEY(C, 0x43),
        DEFINE_KEY(D, 0x44),
        DEFINE_KEY(E, 0x45),
        DEFINE_KEY(F, 0x46),
        DEFINE_KEY(G, 0x47),
        DEFINE_KEY(H, 0x48),
        DEFINE_KEY(I, 0x49),
        DEFINE_KEY(J, 0x4A),
        DEFINE_KEY(K, 0x4B),
        DEFINE_KEY(L, 0x4C),
        DEFINE_KEY(M, 0x4D),
        DEFINE_KEY(N, 0x4E),
        DEFINE_KEY(O, 0x4F),
        DEFINE_KEY(P, 0x50),
        DEFINE_KEY(Q, 0x51),
        DEFINE_KEY(R, 0x52),
        DEFINE_KEY(S, 0x53),
        DEFINE_KEY(T, 0x54),
        DEFINE_KEY(U, 0x55),
        DEFINE_KEY(V, 0x56),
        DEFINE_KEY(W, 0x57),
        DEFINE_KEY(X, 0x58),
        DEFINE_KEY(Y, 0x59),
        DEFINE_KEY(Z, 0x5A),

        DEFINE_KEY(LWIN, 0x5B),
        DEFINE_KEY(RWIN, 0x5C),
        DEFINE_KEY(APPS, 0x5D),

        DEFINE_KEY(SLEEP, 0x5F),

        DEFINE_KEY(NUMPAD0, 0x60),
        DEFINE_KEY(NUMPAD1, 0x61),
        DEFINE_KEY(NUMPAD2, 0x62),
        DEFINE_KEY(NUMPAD3, 0x63),
        DEFINE_KEY(NUMPAD4, 0x64),
        DEFINE_KEY(NUMPAD5, 0x65),
        DEFINE_KEY(NUMPAD6, 0x66),
        DEFINE_KEY(NUMPAD7, 0x67),
        DEFINE_KEY(NUMPAD8, 0x68),
        DEFINE_KEY(NUMPAD9, 0x69),
        DEFINE_KEY(MULTIPLY, 0x6A),
        DEFINE_KEY(ADD, 0x6B),
        DEFINE_KEY(SEPARATOR, 0x6C),
        DEFINE_KEY(SUBTRACT, 0x6D),
        DEFINE_KEY(DECIMAL, 0x6E),
        DEFINE_KEY(DIVIDE, 0x6F),
        DEFINE_KEY(F1, 0x70),
        DEFINE_KEY(F2, 0x71),
        DEFINE_KEY(F3, 0x72),
        DEFINE_KEY(F4, 0x73),
        DEFINE_KEY(F5, 0x74),
        DEFINE_KEY(F6, 0x75),
        DEFINE_KEY(F7, 0x76),
        DEFINE_KEY(F8, 0x77),
        DEFINE_KEY(F9, 0x78),
        DEFINE_KEY(F10, 0x79),
        DEFINE_KEY(F11, 0x7A),
        DEFINE_KEY(F12, 0x7B),
        DEFINE_KEY(F13, 0x7C),
        DEFINE_KEY(F14, 0x7D),
        DEFINE_KEY(F15, 0x7E),
        DEFINE_KEY(F16, 0x7F),
        DEFINE_KEY(F17, 0x80),
        DEFINE_KEY(F18, 0x81),
        DEFINE_KEY(F19, 0x82),
        DEFINE_KEY(F20, 0x83),
        DEFINE_KEY(F21, 0x84),
        DEFINE_KEY(F22, 0x85),
        DEFINE_KEY(F23, 0x86),
        DEFINE_KEY(F24, 0x87),

        DEFINE_KEY(NUMLOCK, 0x90),
        DEFINE_KEY(SCROLL, 0x91),

        DEFINE_KEY(NUMPAD_EQUAL, 0x92),

        DEFINE_KEY(LSHIFT, 0xA0),
        DEFINE_KEY(RSHIFT, 0xA1),
        DEFINE_KEY(LCONTROL, 0xA2),
        DEFINE_KEY(RCONTROL, 0xA3),
        DEFINE_KEY(LMENU, 0xA4),
        DEFINE_KEY(RMENU, 0xA5),

        DEFINE_KEY(SEMICOLON, 0xBA),
        DEFINE_KEY(PLUS, 0xBB),
        DEFINE_KEY(COMMA, 0xBC),
        DEFINE_KEY(MINUS, 0xBD),
        DEFINE_KEY(PERIOD, 0xBE),
        DEFINE_KEY(SLASH, 0xBF),
        DEFINE_KEY(GRAVE, 0xC0),

        SIZE
    };

    enum class MouseButton
    {
        MOUSE_LEFT,
        MOUSE_RIGHT,
        MOUSE_MIDDLE,
        SIZE
    };

#ifdef DEBUG
    inline const char* KeyToShortString(Key key)
    {
        switch (key)
        {
        case Key::KEY_BACKSPACE: return "BACKSPACE";
        case Key::KEY_ENTER: return "ENTER";
        case Key::KEY_TAB: return "TAB";
        case Key::KEY_SHIFT: return "SHIFT";
        case Key::KEY_CONTROL: return "CONTROL";
        case Key::KEY_PAUSE: return "PAUSE";
        case Key::KEY_CAPITAL: return "CAPITAL";
        case Key::KEY_ESCAPE: return "ESCAPE";
        case Key::KEY_CONVERT: return "CONVERT";
        case Key::KEY_NONCONVERT: return "NONCONV";
        case Key::KEY_ACCEPT: return "ACCEPT";
        case Key::KEY_MODECHANGE: return "MODECHNG";
        case Key::KEY_SPACE: return "SPACE";
        case Key::KEY_PRIOR: return "PRIOR";
        case Key::KEY_NEXT: return "NEXT";
        case Key::KEY_END: return "END";
        case Key::KEY_HOME: return "HOME";
        case Key::KEY_LEFT: return "LEFT";
        case Key::KEY_UP: return "UP";
        case Key::KEY_RIGHT: return "RIGHT";
        case Key::KEY_DOWN: return "DOWN";
        case Key::KEY_SELECT: return "SELECT";
        case Key::KEY_PRINT: return "PRINT";
        case Key::KEY_EXEC: return "EXEC";
        case Key::KEY_SNAPSHOT: return "SNAPSHOT";
        case Key::KEY_INSERT: return "INSERT";
        case Key::KEY_DEL: return "DELETE";
        case Key::KEY_HELP: return "HELP";
        case Key::KEY_A: return "A";
        case Key::KEY_B: return "B";
        case Key::KEY_C: return "C";
        case Key::KEY_D: return "D";
        case Key::KEY_E: return "E";
        case Key::KEY_F: return "F";
        case Key::KEY_G: return "G";
        case Key::KEY_H: return "H";
        case Key::KEY_I: return "I";
        case Key::KEY_J: return "J";
        case Key::KEY_K: return "K";
        case Key::KEY_L: return "L";
        case Key::KEY_M: return "M";
        case Key::KEY_N: return "N";
        case Key::KEY_O: return "O";
        case Key::KEY_P: return "P";
        case Key::KEY_Q: return "Q";
        case Key::KEY_R: return "R";
        case Key::KEY_S: return "S";
        case Key::KEY_T: return "T";
        case Key::KEY_U: return "U";
        case Key::KEY_V: return "V";
        case Key::KEY_W: return "W";
        case Key::KEY_X: return "X";
        case Key::KEY_Y: return "Y";
        case Key::KEY_Z: return "Z";
        case Key::KEY_LWIN: return "LWIN";
        case Key::KEY_RWIN: return "RWIN";
        case Key::KEY_APPS: return "APPS";
        case Key::KEY_SLEEP: return "SLEEP";
        case Key::KEY_NUMPAD0: return "NUMPAD0";
        case Key::KEY_NUMPAD1: return "NUMPAD1";
        case Key::KEY_NUMPAD2: return "NUMPAD2";
        case Key::KEY_NUMPAD3: return "NUMPAD3";
        case Key::KEY_NUMPAD4: return "NUMPAD4";
        case Key::KEY_NUMPAD5: return "NUMPAD5";
        case Key::KEY_NUMPAD6: return "NUMPAD6";
        case Key::KEY_NUMPAD7: return "NUMPAD7";
        case Key::KEY_NUMPAD8: return "NUMPAD8";
        case Key::KEY_NUMPAD9: return "NUMPAD9";
        case Key::KEY_MULTIPLY: return "MULTIPLY";
        case Key::KEY_ADD: return "ADD";
        case Key::KEY_SEPARATOR: return "SEPARATOR";
        case Key::KEY_SUBTRACT: return "SUBTRACT";
        case Key::KEY_DECIMAL: return "DECIMAL";
        case Key::KEY_DIVIDE: return "DIVIDE";
        case Key::KEY_F1: return "F1";
        case Key::KEY_F2: return "F2";
        case Key::KEY_F3: return "F3";
        case Key::KEY_F4: return "F4";
        case Key::KEY_F5: return "F5";
        case Key::KEY_F6: return "F6";
        case Key::KEY_F7: return "F7";
        case Key::KEY_F8: return "F8";
        case Key::KEY_F9: return "F9";
        case Key::KEY_F10: return "F10";
        case Key::KEY_F11: return "F11";
        case Key::KEY_F12: return "F12";
        case Key::KEY_F13: return "F13";
        case Key::KEY_F14: return "F14";
        case Key::KEY_F15: return "F15";
        case Key::KEY_F16: return "F16";
        case Key::KEY_F17: return "F17";
        case Key::KEY_F18: return "F18";
        case Key::KEY_F19: return "F19";
        case Key::KEY_F20: return "F20";
        case Key::KEY_F21: return "F21";
        case Key::KEY_F22: return "F22";
        case Key::KEY_F23: return "F23";
        case Key::KEY_F24: return "F24";
        case Key::KEY_NUMLOCK: return "NUMLOCK";
        case Key::KEY_SCROLL: return "SCROLL";
        case Key::KEY_NUMPAD_EQUAL: return "NP_EQUAL";
        case Key::KEY_LSHIFT: return "LSHIFT";
        case Key::KEY_RSHIFT: return "RSHIFT";
        case Key::KEY_LCONTROL: return "LCTRL";
        case Key::KEY_RCONTROL: return "RCTRL";
        case Key::KEY_LMENU: return "LMENU";
        case Key::KEY_RMENU: return "RMENU";
        case Key::KEY_SEMICOLON: return "SEMICOLON";
        case Key::KEY_PLUS: return "PLUS";
        case Key::KEY_COMMA: return "COMMA";
        case Key::KEY_MINUS: return "MINUS";
        case Key::KEY_PERIOD: return "PERIOD";
        case Key::KEY_SLASH: return "SLASH";
        case Key::KEY_GRAVE: return "GRAVE";
        default: return "UNKNOWN";
        }
    }
#endif
}

END_NAMESPACE
