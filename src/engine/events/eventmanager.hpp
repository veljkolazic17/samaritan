#pragma once
#include <defines.hpp>

#include <engine/events/eventhandlerwrapper.hpp>
#include <objecttemplates/singleton.hpp>

#include <vector>
#include <unordered_map>

BEGIN_NAMESPACE

namespace Events
{
    class EventManager SINGLETON(EventManager)
    {
    private:
        SINGLETON_CONSTRUCTOR(EventManager);
    public:
        void Shutdown();
        void Subscribe(const std::string& eventId, const std::unique_ptr<EventHandlerWrapperInterface>&& handler);
        void Unsubscribe(const std::string& eventId, const std::string handlerName);
        void TriggerEvent(const Event& event);
        void QueueEvent(std::unique_ptr<Event>&& event);
        void DispatchEvents();
    private:
        std::vector<std::unique_ptr<Event>> m_EventsQueue;
        std::unordered_map<std::string, std::vector<std::unique_ptr<EventHandlerWrapperInterface>>> m_Subscribers;
    };
}

END_NAMESPACE