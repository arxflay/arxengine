#pragma once
#include <functional>
#include "UIApp.h"
#include "Event.h"
#include <map>
#include <optional>
#include <cassert>

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS EventHandler final
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
        const std::type_info &info;
    };
public:
    EventHandler(IEventHandlable *evtHandlable)
        : m_eventHandlable(evtHandlable)
    {
    }
private:
    Event::EventFunctionWrapperVec &GetOrCreateEvtHandlers(const std::type_info &typeInfo)
    {
        auto evtHandlersIt = m_handlers.find(typeInfo);
        if (evtHandlersIt == m_handlers.end())
            return m_handlers.emplace(std::make_pair(std::cref(typeInfo), decltype(m_handlers)::mapped_type{})).first->second;

        return evtHandlersIt->second;
    }
public:
    template<typename EventType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (*functor)(EventType&))    
    {
        auto &handlers = GetOrCreateEvtHandlers(typeid(EventType));
        handlers.push_back((Event::EventFunctionWrapper(functor)));
    }

    template<typename EventType>
    void Bind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        auto &handlers = GetOrCreateEvtHandlers(typeid(EventType));
        handlers.push_back((Event::EventFunctionWrapper(functor)));
    }
    
    template<typename EventType, typename ClassType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        auto &handlers = GetOrCreateEvtHandlers(typeid(EventType));
        handlers.push_back((Event::EventFunctionWrapper(memberFunction, member)));
    }
private:
    void Unbind(const std::type_info &evtType, Event::EventFunctionWrapper::FunctionParams funcParams)
    {
        auto evtHandlersIt = m_handlers.find(evtType);
        assert(evtHandlersIt != m_handlers.end());
        auto evtHandlerIt = std::find_if(evtHandlersIt->second.begin(), evtHandlersIt->second.end(), [funcParams](const Event::EventFunctionWrapper &wrapper) { return wrapper.GetOriginalFuncParams() == funcParams;});
        assert(evtHandlerIt != evtHandlersIt->second.end());
        evtHandlersIt->second.erase(evtHandlerIt);
    }
public:
    template<typename EventType>
    void Unbind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char*>(c_func), 0 });
    }

    template<typename EventType>
    void Unbind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char *>(&functor), 0 } );
    }
    
    template<typename EventType, typename ClassType>
    void Unbind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char *>(memberFunction), reinterpret_cast<const char *>(member) });
    }
    
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> RaiseEvent(EventType &&event)
    {
        if (m_sealed)
            return; //ignore all incoming events when sealed

        auto evtHandlers = m_handlers.find(typeid(EventType));
        assert(evtHandlers != m_handlers.end());
        auto &evtHandlersVec = evtHandlers->second; 

        std::decay_t<EventType> evt = std::forward<EventType>(event);
        evt.SetObject(GetEventHandlable());
        evt.SetEvtFuncWrapperIterator(Event::EvtFuncWrapperIterator(evtHandlersVec.rbegin(), evtHandlersVec.rend()));
        UIApp::GetInstance()->EnqueueEvent(std::move(evt));
    }

    IEventHandlable *GetEventHandlable() { return m_eventHandlable; }

    /**
     * @brief Seals EventHandler
     * @description Seals EventHandler forever. Every new raised event is ignored
     */
    void Seal() { m_sealed = true; }

private:
    bool m_sealed;
    IEventHandlable *m_eventHandlable;
    std::map<ComparableTypeInfo, Event::EventFunctionWrapperVec> m_handlers;
};

ARX_NAMESPACE_END
