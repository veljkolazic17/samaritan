#include <engine/events/eventmanager.hpp>
#include <utils/asserts/assert.hpp>

BEGIN_NAMESPACE

namespace Events
{
    void EventManager::Shutdown()
    {
        m_Subscribers.clear();
    }

    void EventManager::Subscribe(const std::string& eventId, const std::unique_ptr<EventHandlerWrapperInterface>&& handler)
    {
        auto subscribers = m_Subscribers.find(eventId);

        if (subscribers != m_Subscribers.end()) 
        {
            auto& handlers = subscribers->second;
            for (auto& it : handlers) {
                if (it->GetType() == handler->GetType()) 
                {
                    softAssert(false, "Trying to reggister callback where one exists!");
                    return;
                }
            }
            handlers.emplace_back(std::move(handler));
        } 
        else 
        {
            m_Subscribers[eventId].emplace_back(std::move(handler));
        }
    }
    
    void EventManager::Unsubscribe(const std::string& eventId, const std::string handlerName)
    {
        std::vector<std::unique_ptr<EventHandlerWrapperInterface>>& handlers = m_Subscribers[eventId];
        for (auto it = handlers.begin(); it != handlers.end(); ++it) 
        {
            if (it->get()->GetType() == handlerName) 
            {
                it = handlers.erase(it);
                return;
            }
        }
    }
    
    void EventManager::TriggerEvent(const Event& event)
    {
        for (auto& handler : m_Subscribers[event.GetEventType()]) 
        {
            handler->Exec(event);
        }
    }
    
    void EventManager::QueueEvent(std::unique_ptr<Event>&& event)
    {
        m_EventsQueue.emplace_back(std::move(event));
    }
    
    void EventManager::DispatchEvents()
    {
        for (auto eventIt = m_EventsQueue.begin(); eventIt != m_EventsQueue.end();) 
        {
            if (!eventIt->get()->m_IsHandled) 
            {
                TriggerEvent(*eventIt->get());
                eventIt = m_EventsQueue.erase(eventIt);
            } 
            else 
            {
                ++eventIt;
            }
        }
    }
}

END_NAMESPACE