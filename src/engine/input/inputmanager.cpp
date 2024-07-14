#include <engine/input/inputmanager.hpp>

#include <engine/memory/memory.hpp>
#include <engine/input/events/keyboardevents.hpp>
#include <engine/input/events/mouseevents.hpp>
#include <engine/events/eventmanager.hpp>

BEGIN_NAMESPACE

namespace Input
{
    void InputManager::Init()
    {
        Memory::Zero(&m_InputStates, sizeof(m_InputStates));
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

        Memory::mmcpy(
            &m_InputStates.m_CurrentKeyboardState, 
            &m_InputStates.m_PrevoiusKeyboardState, 
            sizeof(Key)
        );

        Memory::mmcpy(
            &m_InputStates.m_CurrentMouseState, 
            &m_InputStates.m_PreviousMousetate, 
            sizeof(MouseButton)
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
                std::unique_ptr<KeyboardInputPressedEvent> pressedEvent = std::move(std::make_unique<KeyboardInputPressedEvent>());
                pressedEvent->m_Key = key;
                Events::AddEvent(std::move(pressedEvent));
            }
            else
            {
                std::unique_ptr<KeyboardInputReleasedEvent> releasedEvent = std::move(std::make_unique<KeyboardInputReleasedEvent>());
                releasedEvent->m_Key = key;
                Events::AddEvent(std::move(releasedEvent));
            }
        }
    }

    mbool InputManager::IsKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return;
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
                std::unique_ptr<MouseButtonPressedEvent> pressedEvent = std::move(std::make_unique<MouseButtonPressedEvent>());
                pressedEvent->m_Button = button;
                Events::AddEvent(std::move(pressedEvent));
            }
            else
            {
                std::unique_ptr<MouseButtonReleasedEvent> releasedEvent = std::move(std::make_unique<MouseButtonReleasedEvent>());
                releasedEvent->m_Button = button;
                Events::AddEvent(std::move(releasedEvent));
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

            std::unique_ptr<MouseMovedEvent> movedEvent = std::move(std::make_unique<MouseMovedEvent>());
            movedEvent->m_X = x;
            movedEvent->m_Y = y;
            Events::AddEvent(std::move(movedEvent));
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