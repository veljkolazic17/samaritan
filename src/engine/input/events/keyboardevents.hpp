#pragma once 
#include <defines.hpp>

#include <engine/events/event.hpp>
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    struct KeyboardInputPressedEvent : public Events::Event
    {
        DEFINE_EVENT("KeyboardInputPressed");
        Key m_Key { 0 };
    };

    struct KeyboardInputReleasedEvent : public Events::Event
    {
        DEFINE_EVENT("KeyboardInputReleased");
        Key m_Key { 0 };
    };

}

END_NAMESPACE