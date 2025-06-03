#pragma once
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    struct KeyboardState
    {
        smbool m_Keys[(unsigned long)Key::SIZE];
    };

    struct MouseState
    {
        smint16 m_X;
        smint16 m_Y;
        smbool m_Buttons[(unsigned long)MouseButton::SIZE];
    };

    struct InputState
    {
        KeyboardState m_PrevoiusKeyboardState;
        KeyboardState m_CurrentKeyboardState;
        
        MouseState m_PreviousMousetate;
        MouseState m_CurrentMouseState;
    };
}

END_NAMESPACE