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
        smZero(&m_InputStates, sizeof(m_InputStates));
        m_IsInitialized = true;
    }

    void InputManager::Shutdown()
    {
        m_IsInitialized = false;
    }

    void InputManager::SingletonInit()
    {
        Init();
    }

    void InputManager::SingletonShutdown()
    {
        Shutdown();
    }

    void InputManager::Update(smfloat64 deltaTime)
    {
        if(!m_IsInitialized)
        {
            return;
        }

        smCopy(
            &m_InputStates.m_CurrentKeyboardState, 
            &m_InputStates.m_PrevoiusKeyboardState, 
            (unsigned long)sizeof(Key)
        );

        smCopy(
            &m_InputStates.m_CurrentMouseState, 
            &m_InputStates.m_PreviousMousetate, 
            (unsigned long)sizeof(MouseButton)
        );

        //TODO : XBox controller states
    }

    //Keyboard
    void InputManager::ProcessKey(Key key, smbool isPressed)
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
                LogInfo(LogChannel::Inputs, "Keyboard button pressed!");
            }
            else
            {
                KeyboardInputReleasedEvent releasedEvent;
                releasedEvent.m_Key = key;
                Events::AddEvent<KeyboardInputReleasedEvent>(std::move(releasedEvent));
                LogInfo(LogChannel::Inputs, "Keyboard button released!");
            }
        }
    }

    smbool InputManager::IsKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key];
    }

    smbool InputManager::IsKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_CurrentKeyboardState.m_Keys[(unsigned long)key];
    }

    smbool InputManager::WasKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_PrevoiusKeyboardState.m_Keys[(unsigned long)key];
    }

    smbool InputManager::WasKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_PrevoiusKeyboardState.m_Keys[(unsigned long)key];
    }
    
    //Mouse
    void InputManager::ProcessMouseButton(MouseButton button, smbool isPressed)
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
                LogInfo(LogChannel::Inputs, "Mouse button pressed!");
            }
            else
            {
                MouseButtonReleasedEvent releasedEvent;
                releasedEvent.m_Button = button;
                Events::AddEvent<MouseButtonReleasedEvent>(std::move(releasedEvent));
                LogInfo(LogChannel::Inputs, "Mouse button released!");
            }
        }
    }

    void InputManager::ProcessMouseMovement(smint16 x, smint16 y)
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

    void InputManager::ProcessMouseWheel(smint16 deltaMove)
    {
        if(!m_IsInitialized)
        {
            return;
        }
        //TODO : Implement mouse wheel
    }

    smbool InputManager::IsMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button];
    }

    smbool InputManager::IsMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_CurrentMouseState.m_Buttons[(unsigned long)button];
    }

    smbool InputManager::WasMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return !m_InputStates.m_PreviousMousetate.m_Buttons[(unsigned long)button];
    }

    smbool InputManager::WasMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
        return m_InputStates.m_PreviousMousetate.m_Buttons[(unsigned long)button];
    }
}

END_NAMESPACE