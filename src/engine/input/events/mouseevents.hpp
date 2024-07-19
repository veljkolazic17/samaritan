#pragma once 
#include <defines.hpp>

#include <engine/events/event.hpp>
#include <engine/input/input.hpp>

BEGIN_NAMESPACE

namespace Input
{
    struct MouseButtonPressedEvent : public Events::Event
    {
        DEFINE_EVENT("MouseButtonPressed");
        MouseButton m_Button { 0 };
    };

    struct MouseButtonReleasedEvent : public Events::Event
    {
        DEFINE_EVENT("MouseButtonReleased");
        MouseButton m_Button { 0 };
    };

    struct MouseMovedEvent : public Events::Event
    {
        DEFINE_EVENT("MouseMoved");
        muint16 m_X { 0 };
        muint16 m_Y { 0 };
    };
    
}

END_NAMESPACE