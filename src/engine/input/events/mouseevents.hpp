#pragma once 
#include <defines.hpp>

#include <engine/events/event.hpp>
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    class MouseButtonPressedEvent : public Events::Event
    {
    public:
        DEFINE_EVENT("MouseButtonPressed")

        MouseButtonPressedEvent(MouseButton key)
            : m_Key(key)
        {
        }  

        MouseButton m_Key { 0 };
    };

    class MouseButtonReleasedEvent : public Events::Event
    {
    public:
        DEFINE_EVENT("MouseButtonReleased")

        MouseButtonReleasedEvent(MouseButton key)
            : m_Key(key)
        {
        }  

        MouseButton m_Key { 0 };
    };

    class MouseMovedEvent : public Events::Event
    {
    public:
        DEFINE_EVENT("MouseMoved")

        MouseMovedEvent(muint16 x, muint16 y)
            : m_X(x)
            , m_Y(y)
        {
        }  

        muint16 m_X { 0 };
        muint16 m_Y { 0 };
    };
    
}

END_NAMESPACE