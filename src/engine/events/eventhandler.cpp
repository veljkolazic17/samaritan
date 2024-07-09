#include <engine/events/eventhandler.hpp>

BEGIN_NAMESPACE

namespace Events
{
	void EventHandler::Init()
	{

	}

	void EventHandler::Shutdown()
	{

	}

	template<typename T>
	mbool EventHandler::RegisterEvent(EventCode eventCode, const T& listener, EventCallback<T> callback)
	{

	}

	template<typename T>
	mbool EventHandler::FireEvent(const T& caller, EventContext& context)
	{

	}
}

END_NAMESPACE