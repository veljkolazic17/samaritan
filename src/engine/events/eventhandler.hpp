#pragma once
#include <engine/events/event.hpp>
#include <objecttemplates/singleton.hpp>

BEGIN_NAMESPACE

namespace Events
{
	class EventHandler SINGLETON(EventHandler)
	{
		void Init();
		void Shutdown();

		template<typename T>
		mbool RegisterEvent(EventCode eventCode, const T& listener, EventCallback<T> callback);
		template<typename T>
		mbool FireEvent(const T& caller, EventContext& context);

	private:
		SINGLETON_CONSTRUCTOR(EventHandler)
		Event m_Events[EventCode::SIZE];
	};
}

END_NAMESPACE