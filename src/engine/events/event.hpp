#pragma once
#include <defines.hpp>

#define EVENT_CONTEXT_SIZE	128
#define MAX_EVENT_CODES		16384
#define InvalidCallback		nullptr
#define InvalidCaller		nullptr

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