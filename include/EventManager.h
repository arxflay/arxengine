#pragma once
#include "ArxDefines.h"
#include <type_traits>
#include <functional>
#include <map>
#include <vector>
#include <optional>
#include "logging/Logger.h"
#include "Event.h"
#include "UIApp.h"
#include "UIEventLoop.h"

ARX_NAMESPACE_BEGIN

struct ComparableTypeInfo
{
    ComparableTypeInfo(const std::type_info &infoIn)
        : info(infoIn)
    {
    }

    bool operator>(const ComparableTypeInfo &b) const { return info.hash_code() > b.info.hash_code(); }
    bool operator<(const ComparableTypeInfo &b) const { return info.hash_code() < b.info.hash_code(); }
    const std::type_info &info;
};


class EventsHandlersMap : public std::map<ComparableTypeInfo, EventHandlersVector>
{
public:
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>, std::optional<EventHandlersVectorRef>> FindEventHandlers()
    {
        auto evtHandlersIt = find(typeid(EventType));
        return (evtHandlersIt == end()) ? std::nullopt : std::make_optional<EventHandlersVectorRef>(std::ref(evtHandlersIt->second));
    }

    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>, std::optional<EventHandlersVectorRef>> FindOrCreateEventHandlers()
    {
        std::optional<EventHandlersVectorRef> eventHandlers = FindEventHandlers<EventType>();
        if (!eventHandlers.has_value())
        {
            auto createdEventHandlersPair = emplace(std::make_pair(ComparableTypeInfo(typeid(EventType)), EventHandlersVector{}));
            if (!createdEventHandlersPair.second)
                GLOG->Error("Failed to create EventHandlers for Event %s", typeid(EventType).name());
            else
                eventHandlers = std::ref(createdEventHandlersPair.first->second);
        }
        return eventHandlers;
    }

    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>, std::optional<std::reference_wrapper<const EventHandler>>> GetEventHandlerByAddr(EventHandler::UnwrappedFuncAddrs funcParams)
    {
        std::optional<EventHandlersVectorRef> eventHandlers = FindEventHandlers<EventType>();
        if (!eventHandlers.has_value())
            return std::nullopt;
        
        auto evtHandlerIt = std::find_if(eventHandlers->get().begin(), eventHandlers->get().end(), [funcParams](const EventHandler &handler) { 
            return handler.GetOriginalFuncParams() == funcParams;
        });

        return (evtHandlerIt == eventHandlers->get().end()) ? std::nullopt : std::make_optional<std::reference_wrapper<const EventHandler>>(std::cref(*evtHandlerIt));
    }
    
    //remove first match from end
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>, bool> RemoveFirstHandlerMatch(EventHandler::UnwrappedFuncAddrs funcParams)
    {
        std::optional<EventHandlersVectorRef> eventHandlers = FindEventHandlers<EventType>();
        if (!eventHandlers.has_value())
            return false;

        auto evtHandlerIt = std::find_if(eventHandlers->get().rbegin(), eventHandlers->get().rend(), [funcParams](const EventHandler &wrapper) { 
            return wrapper.GetOriginalFuncParams() == funcParams;
        });

        if (evtHandlerIt == eventHandlers->get().end())
            return false;

        eventHandlers->get().erase(evtHandlerIt);
        return true;
    }
};

/**
 * @class EventManager
 * @brief Class that is responsible for event managment
 */
class EventManager final
{
public:
    template<typename EventType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        auto eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>();
        eventHandlers()->get().emplace_back(c_func);
    }

    template<typename EventType>
    void Bind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        auto eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>();
        eventHandlers->get().emplace_back(functor);
    }

    template<typename EventType, typename ClassType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        auto eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>();
        eventHandlers->get().emplace_back(memberFunction, member);
    }
private:
    template<typename EventType>
    bool Unbind(EventHandler::UnwrappedFuncAddrs params)
    {
        if (m_eventsHandlersMap.RemoveFirstHandlerMatch<EventType>())
        {
            GLOG->Debug("EventHandler with fn_addr=%p, instance_addr=%p for event %s not found or couldn't be removed", params.addr, params.instance_addr, typeid(EventType).name());
            return false;
        }

        return true;
    }
public:
    template<typename EventType>
    bool Unbind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        return Unbind<EventType>(EventHandler::UnwrappedFuncAddrs{ reinterpret_cast<const char*>(c_func), 0 });
    }

    template<typename EventType>
    bool Unbind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        return Unbind<EventType>(EventHandler::UnwrappedFuncAddrs{ reinterpret_cast<const char *>(&functor), 0 } );
    }

    template<typename EventType, typename ClassType>
    bool Unbind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        return Unbind<EventType>(EventHandler::UnwrappedFuncAddrs{ reinterpret_cast<const char *>(memberFunction), reinterpret_cast<const char *>(member) });
    }

    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> QueueEvent(std::unique_ptr<Event> &&event)
    {
        event->m_eventHandlersPtr = m_eventsHandlersMap.FindEventHandlers<EventType>();
        UIApp::GetGlobalApp()->GetEventLoop().EnqueueEvent(std::move(event));
    }

private:
    EventsHandlersMap m_eventsHandlersMap;
};

ARX_NAMESPACE_END
