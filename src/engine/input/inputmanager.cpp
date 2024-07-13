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
                Events::AddEvent(std::move(pressedEvent));
            }
            else
            {
                std::unique_ptr<KeyboardInputReleasedEvent> releasedEvent = std::move(std::make_unique<KeyboardInputReleasedEvent>());
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
    }

    mbool InputManager::IsKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }

    mbool InputManager::WasKeyUp(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }

    mbool InputManager::WasKeyDown(Key key)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }
    
    //Mouse
    void InputManager::ProcessMouseButton(MouseButton button, mbool isPressed)
    {
        if(!m_IsInitialized)
        {
            return;
        }
    }

    void InputManager::ProcessMouseMovement(mint16 x, mint16 y)
    {
        if(!m_IsInitialized)
        {
            return;
        }
    }

    void InputManager::ProcessMouseWheel(mint16 deltaMove)
    {
        if(!m_IsInitialized)
        {
            return;
        }
    }

    mbool InputManager::IsMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }

    mbool InputManager::IsMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }

    mbool InputManager::WasMouseButtonUp(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }

    mbool InputManager::WasMouseButtonDown(MouseButton button)
    {
        if(!m_IsInitialized)
        {
            return false;
        }
    }
}

END_NAMESPACE