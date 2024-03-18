/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_EVENT_MANAGER_H
#define ARX_EVENT_MANAGER_H
#include "arxengine/ArxDefines.h"
#include <type_traits>
#include <functional>
#include <map>

#include "arxengine/evt/Event.h"
#include <vector>
#include <optional>
#include "arxengine/logging/Logger.h"
#include "arxengine/GameApp.h"
#include "arxengine/evt/EventProcessor.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

namespace detail
{
class EventHandler
{
    public:
        struct UnwrappedFuncAddrs
        {
            constexpr bool operator==(const UnwrappedFuncAddrs &b) const
            {
                return addr == b.addr && instance_addr == b.instance_addr; 
            }
            uintptr_t addr;
            uintptr_t instance_addr;
        };

        template<typename EventType>
        constexpr EventHandler(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))
        {
            m_func = [c_func](Event& evt) constexpr -> void { c_func(static_cast<EventType&>(evt)); };
            m_originalFuncAddress = UnwrappedFuncAddrs{ reinterpret_cast<uintptr_t>(c_func), 0 };
        }

        template<typename EventType>
        EventHandler(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor) 
        {
            m_func = [functor](Event& evt) constexpr -> void { functor(static_cast<EventType&>(evt)); };
            m_originalFuncAddress = UnwrappedFuncAddrs { reinterpret_cast<uintptr_t>(&functor), 0 };
        }

        template<typename EventType, typename ClassType>
        constexpr EventHandler(std::enable_if_t<is_event_type_v<EventType>>(ClassType::*memberFunction)(EventType&), ClassType *instance)
        {
            m_func = [instance, memberFunction](Event& evt) constexpr -> void { (*instance.*memberFunction)(static_cast<EventType&>(evt)); };
            
            /*hacky workaround*/
            union
            {
                void (ClassType::*memberFunction)(EventType&);
                uintptr_t addr;
            } memberFunctionAddr;
            
            memberFunctionAddr.memberFunction = memberFunction;
            m_originalFuncAddress = UnwrappedFuncAddrs{ memberFunctionAddr.addr, reinterpret_cast<uintptr_t>(instance) };
            
        }

        void operator()(Event &evt) const //do not call directly, unsafe
        {
            m_func(evt);
        }

        constexpr bool operator==(const EventHandler &wrapper) const
        {
            return GetOriginalFuncParams() == wrapper.GetOriginalFuncParams();
        }

        constexpr const UnwrappedFuncAddrs &GetOriginalFuncParams() const
        {
            return m_originalFuncAddress;
        }

    private:
        UnwrappedFuncAddrs m_originalFuncAddress;
        std::function<void(Event&)> m_func;
    };
}

using EventHandlersVector = std::vector<detail::EventHandler>;
using EventHandlersVectorRef = std::reference_wrapper<EventHandlersVector>;
using EventHandlersVectorCref = std::reference_wrapper<const EventHandlersVector>;

/**
 * @class EventManager
 * @brief Class that is responsible for event managment
 */
class EventManager final
{
private:
    struct ComparableTypeInfo
    {
        ComparableTypeInfo(const std::type_info &infoIn)
            : info(infoIn)
        {
        }

        bool operator>(const ComparableTypeInfo &b) const { return info.hash_code() > b.info.hash_code(); }
        bool operator<(const ComparableTypeInfo &b) const { return info.hash_code() < b.info.hash_code(); }
        bool operator==(const ComparableTypeInfo &b) const { return info.hash_code() == b.info.hash_code(); }
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
        std::enable_if_t<is_event_type_v<EventType>, std::optional<std::reference_wrapper<const detail::EventHandler>>> GetEventHandlerByAddr(detail::EventHandler::UnwrappedFuncAddrs funcParams)
        {
            std::optional<EventHandlersVectorRef> eventHandlers = FindEventHandlers<EventType>();
            if (!eventHandlers.has_value())
                return std::nullopt;
            
            auto evtHandlerIt = std::find_if(eventHandlers->get().begin(), eventHandlers->get().end(), [funcParams](const detail::EventHandler &handler) { 
                return handler.GetOriginalFuncParams() == funcParams;
            });

            return (evtHandlerIt == eventHandlers->get().end()) ? std::nullopt : std::make_optional<std::reference_wrapper<const detail::EventHandler>>(std::cref(*evtHandlerIt));
        }
        
        //remove first match from end
        template<typename EventType>
        std::enable_if_t<is_event_type_v<EventType>, bool> RemoveFirstHandlerMatch(detail::EventHandler::UnwrappedFuncAddrs funcParams)
        {
            std::optional<EventHandlersVectorRef> eventHandlers = FindEventHandlers<EventType>();
            if (!eventHandlers.has_value())
                return false;

            auto evtHandlerIt = std::find_if(eventHandlers->get().rbegin(), eventHandlers->get().rend(), [funcParams](const detail::EventHandler &wrapper) { 
                return wrapper.GetOriginalFuncParams() == funcParams;
            });

            if (evtHandlerIt == eventHandlers->get().rend())
                return false;

            eventHandlers->get().erase(evtHandlerIt);
            return true;
        }
    };
public:
    EventManager(ArxObject *owner)
        : m_owner(owner)
    {
    }

    void ReplaceWithCopy(const EventManager &manager)
    {
        m_eventsHandlersMap = manager.m_eventsHandlersMap;
    }

    EventManager(const EventManager&) = delete;
    EventManager(EventManager&&) = delete;
    EventManager &operator=(const EventManager&) = delete;
    EventManager &operator=(EventManager&&) = delete;

    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>, bool> HasNonDefaultEventHandler()
    {
        auto eventHandler = m_eventsHandlersMap.FindEventHandlers<EventType>();
        return eventHandler.has_value() && !eventHandler->get().empty();
    }

    template<typename EventType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        auto &eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>().value().get();
        eventHandlers.emplace_back(c_func);
    }

    template<typename EventType>
    void Bind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        auto &eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>().value().get();
        eventHandlers.emplace_back(functor);
    }

    template<typename EventType, typename ClassType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        auto &eventHandlers = m_eventsHandlersMap.FindOrCreateEventHandlers<EventType>().value().get();
        eventHandlers.emplace_back(memberFunction, member);
    }
private:
    template<typename EventType>
    bool Unbind(detail::EventHandler::UnwrappedFuncAddrs params)
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
        return Unbind<EventType>(detail::EventHandler::UnwrappedFuncAddrs{ reinterpret_cast<uintptr_t>(c_func), 0 });
    }

    template<typename EventType>
    bool Unbind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        return Unbind<EventType>(detail::EventHandler::UnwrappedFuncAddrs{ reinterpret_cast<uintptr_t>(&functor), 0 } );
    }

    template<typename EventType, typename ClassType>
    bool Unbind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        /*hacky workaround*/
        union
        {
            void (ClassType::*memberFunction)(EventType&);
            uintptr_t addr;
        } memberFunctionAddr;
        memberFunctionAddr.memberFunction = memberFunction;
        return Unbind<EventType>(detail::EventHandler::UnwrappedFuncAddrs{ memberFunctionAddr.addr, reinterpret_cast<uintptr_t>(member) });
    }

    //m_eventHandlers vector can be changed after queue event 
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> QueueEvent(std::unique_ptr<Event> &&event)
    {
        event->m_getEventHandlersFn = [this]() { return m_eventsHandlersMap.FindEventHandlers<EventType>(); };
        event->SetSender(m_owner);
        GameApp::GetGlobalApp()->GetEventProcessor().EnqueueEvent(std::move(event));
    }

    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> ScheduleEvent(std::unique_ptr<Event> &&event)
    {
        event->m_getEventHandlersFn = [this]() { return m_eventsHandlersMap.FindEventHandlers<EventType>(); };        
        event->SetSender(m_owner);
        GameApp::GetGlobalApp()->GetEventProcessor().ScheduleEvent(std::move(event));
    }
private:
    class InternalEventParamsUnsetter final
    {
    public:
        friend class EventManager;
        InternalEventParamsUnsetter(Event &e)
            : m_event(e)
        {
        
        }

        ~InternalEventParamsUnsetter()
        {
            m_event.SetSender(nullptr);
            m_event.m_getEventHandlersFn = decltype(m_event.m_getEventHandlersFn){};
        }
    private:
        Event &m_event;
    };
public:
    
    /*process event immediately without enqueue into event queue*/
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> ProcessEvent(Event &event)
    {
        InternalEventParamsUnsetter unsetter(event);
        event.SetSender(m_owner);
        event.m_getEventHandlersFn = [this]() { return m_eventsHandlersMap.FindEventHandlers<EventType>(); };
        GameApp::GetGlobalApp()->GetEventProcessor().ProcessEvent(event);
    }

private:
    ArxObject *m_owner;
    EventsHandlersMap m_eventsHandlersMap;
};

ARX_NAMESPACE_END
#endif
