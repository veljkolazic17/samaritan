#pragma once
#include <defines.hpp>

#include <engine/events/eventhandlerwrapper.hpp>
#include <objecttemplates/singleton.hpp>
#include <objecttemplates/shutdownable.hpp>

#include <vector>
#include <unordered_map>
#include <memory>

BEGIN_NAMESPACE

namespace Events
{
    class EventManager SINGLETON(EventManager), public Shutdownable
    {
    private:
        SINGLETON_CONSTRUCTOR(EventManager);
    public:
        void Init() override;
        void Shutdown() override;
        void Subscribe(const std::string& eventId, std::unique_ptr<EventHandlerWrapperInterface>&& handler);
        void Unsubscribe(const std::string& eventId, const std::string handlerName);
        void TriggerEvent(const Event& event);
        void AddEvent(std::unique_ptr<Event>&& event);
        void DispatchEvents();
    private:
        std::vector<std::unique_ptr<Event>> m_EventsQueue;
        std::unordered_map<std::string, std::vector<std::unique_ptr<EventHandlerWrapperInterface>>> m_Subscribers;
    };

    template<typename EventType>
    SM_INLINE void AddEvent(EventType&& event)
    {
        std::unique_ptr<EventType> pressedEvent = std::move(std::make_unique<EventType>(event));
        EventManager::GetInstance().AddEvent(std::forward<std::unique_ptr<EventType>>(pressedEvent));
    }

    SM_INLINE void TriggerEvent(const Event& event)
    {
        EventManager::GetInstance().TriggerEvent(event);
    }
}

END_NAMESPACE