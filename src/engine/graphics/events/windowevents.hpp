#pragma once 
#include <defines.hpp>

#include <engine/events/event.hpp>

BEGIN_NAMESPACE

namespace Graphics
{
    struct WindowResizedEvent : public Events::Event
    {
        DEFINE_EVENT("WindowResizedEvent");
        muint32 m_Width { 0 };
        muint32 m_Height { 0 };
    };

    struct WindowClosedEvent : public Events::Event
    {
        DEFINE_EVENT("WindowClosedEvent");
    };
}

END_NAMESPACE