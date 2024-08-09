#ifdef TARGET_WIN
#pragma once
#include <defines.hpp>
#include <engine/input/input.hpp>
#include <engine/input/inputmanager.hpp>

#include <windows.h>
#include <windowsx.h> 

BEGIN_NAMESPACE

namespace Input::Win
{
    SM_INLINE void ProcessMouseButtons(muint32 msg)
	{
        mbool isPressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
        switch (msg)
		{
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            {
                InputManager::GetInstance().ProcessMouseButton(Input::MouseButton::MOUSE_LEFT, isPressed);
            }
            break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
            {
                InputManager::GetInstance().ProcessMouseButton(Input::MouseButton::MOUSE_MIDDLE, isPressed);
            }
            break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
            {
                InputManager::GetInstance().ProcessMouseButton(Input::MouseButton::MOUSE_RIGHT, isPressed);
            }
            break;
		}
	}

    SM_INLINE void ProcessKeyboardButtons(muint32 msg, WPARAM w_param)
    {
        mbool isPressed = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
        InputManager::GetInstance().ProcessKey((Input::Key)w_param, isPressed);
    }

    SM_INLINE void ProcessMouseMove(LPARAM l_param)
    {
        InputManager::GetInstance().ProcessMouseMovement(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
    }

    SM_INLINE void ProcessMouseWheel(muint32 msg)
    {

    }
}

END_NAMESPACE
#endif
