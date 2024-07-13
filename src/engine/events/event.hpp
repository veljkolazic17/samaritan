#pragma once
#include <defines.hpp>

BEGIN_NAMESPACE

namespace Events
{
	class Event 
	{
	public:
		virtual const std::string GetEventType() const = 0;

	public:
		bool m_IsHandled { false };
	};
}

END_NAMESPACE

#define DEFINE_EVENT(eventType)                		\                                        
    const std::string GetEventType() const override 	\
    {                                           	\
        return eventType;            				\
    }
