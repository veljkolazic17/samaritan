#pragma once
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    struct KeyboardState
    {
        mbool m_Keys[(unsigned long)Key::SIZE];
    };

    struct MouseState
    {
        mint16 m_X;
        mint16 m_Y;
        mbool m_Buttons[(unsigned long)MouseButton::SIZE];
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