#include <engine/input/inputmanager.hpp>

#include <engine/memory/memory.hpp>
#include <engine/input/events/keyboardevents.hpp>
#include <engine/input/events/mouseevents.hpp>
#include <engine/events/eventmanager.hpp>

#ifdef DEBUG
#include <utils/logger/log.hpp>
#endif

#include <memory>

BEGIN_NAMESPACE

namespace Input
{
    void InputManager::Init()
    {
        Zero(&m_InputStates, sizeof(m_InputStates));
        m_IsInitialized = true;
    }

    void InputManager::Shutdown()
    {
        m_IsInitialized = false;
    }

    void InputManager::Update(mfloat64 deltaTime)
    {
        if(!m_IsInitialized)
        {
            return;
        }

        Copy(
            &m_InputStates.m_CurrentKeyboardState, 
            &m_InputStates.m_PrevoiusKeyboardState, 
            (unsigned long)sizeof(Key)
        );

        Copy(
            &m_InputStates.m_CurrentMouseState, 
            &m_InputStates.m_PreviousMousetate, 
            (unsigned long)sizeof(MouseButton)
        );

        //TODO : XBox controller states
    }

    //Keyboard
    void InputManager::ProcessKey(Key key, mbool isPressed)
    {
        if(!m_IsInitialized)
        {
            return;
        }
        if(m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key] != isPressed)
        {
            m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key] = isPressed;

            if(isPressed)
            {
                KeyboardInputPressedEvent pressedEvent;
                pressedEvent.m_Key = key;
                Events::AddEvent<KeyboardInputPressedEvent>(std::move(pressedEvent));
                LogInfoTTY("Keyboard button pressed!");
            }
            else
            {
                KeyboardInputReleasedEvent releasedEvent;
                releasedEvent.m_Key = key;
                Events::AddEvent<KeyboardInputReleasedEvent>(std::move(releasedEvent));
                LogInfoTTY("Keyboard button released!");
            }
        }
    }

    mbool InputManager::IsKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key];
    }

    mbool InputManager::IsKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key];
    }

    mbool InputManager::WasKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_PrevoiusKeyboardState.m_Keys[(unsigned long)key];
    }

    mbool InputManager::WasKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_PrevoiusKeyboardState.m_Keys[(unsigned long)key];
    }
    
    //Mouse
    void InputManager::ProcessMouseButton(MouseButton button, mbool isPressed)
    {
        if(!m_IsInitialized)
        {
            return;
        }
        if(m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button] != isPressed)
        {
            m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button] = isPressed;

            if(isPressed)
            {
                MouseButtonPressedEvent pressedEvent;
                pressedEvent.m_Button = button;
                Events::AddEvent<MouseButtonPressedEvent>(std::move(pressedEvent));
                LogInfoTTY("Mouse button pressed!");
            }
            else
            {
                MouseButtonReleasedEvent releasedEvent;
                releasedEvent.m_Button = button;
                Events::AddEvent<MouseButtonReleasedEvent>(std::move(releasedEvent));
                LogInfoTTY("Mouse button released!");
            }
        }
    }

    void InputManager::ProcessMouseMovement(mint16 x, mint16 y)
    {
        if(!m_IsInitialized)
        {
            return;
        }
        if(m_InputStates.m_CurrentMouseState.m_X != x || m_InputStates.m_CurrentMouseState.m_Y != y)
        {
            m_InputStates.m_CurrentMouseState.m_X = x;
            m_InputStates.m_CurrentMouseState.m_Y = y;

            MouseMovedEvent movedEvent;
            movedEvent.m_X = x;
            movedEvent.m_Y = y;
            Events::AddEvent<MouseMovedEvent>(std::move(movedEvent));
        }
    }

    void InputManager::ProcessMouseWheel(mint16 deltaMove)
    {
        if(!m_IsInitialized)
        {
            return;
        }
        //TODO : Implement mouse wheel
    }

    mbool InputManager::IsMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button];
    }

    mbool InputManager::IsMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button];
    }

    mbool InputManager::WasMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_PreviousMousetate.m_Buttons[(unsigned long)button];
    }

    mbool InputManager::WasMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_PreviousMousetate.m_Buttons[(unsigned long)button];
    }
}

END_NAMESPACE