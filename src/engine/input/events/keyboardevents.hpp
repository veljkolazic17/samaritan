#pragma once 
#include <defines.hpp>

#include <engine/events/event.hpp>
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    class KeyboardInputPressedEvent : public Events::Event
    {
    public:
        DEFINE_EVENT("KeyboardInputPressed")

        KeyboardInputPressedEvent(Key key)
            : m_Key(key)
        {
        }  

        Key m_Key { 0 };
    };

    class KeyboardInputReleasedEvent : public Events::Event
    {
    public:
        DEFINE_EVENT("KeyboardInputReleased")

        KeyboardInputReleasedEvent(Key key)
            : m_Key(key)
        {
        }  

        Key m_Key { 0 };
    };

}

END_NAMESPACE