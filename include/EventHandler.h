#pragma once
#include <functional>
#include "UIApp.h"
#include "Event.h"
#include <map>
#include <optional>
#include <cassert>

ARX_NAMESPACE_BEGIN

/**
 * @class EventHandler
 * @brief Class that is responsible for event managment
 */
class ARX_EXPORTS EventHandler final
{
private:
    /**
     * @class ComparableTypeInfo
     * @brief Used for EventType event handler differention
     * @description Compares typeinfo by hash code
     */
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
    /**
     * @brief EventHandler constructor
     * @description evtHandlable is expected to be not nullptr otherwise exception is thrown
     * Also evtHandlable must be a parent of EventHandler or undefined behavior is expected
     * @param[in] evtHandlable EventHandler parent 
     */
    EventHandler(IEventHandlable *evtHandlable)
        : m_sealed(false), m_eventHandlable(evtHandlable)
    {
        assert(evtHandlable != nullptr);
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
    /**
     * @brief Bind event handler for EventType 
     * @description This overload binds event handler c-style function  
     * @param[in] c_func c-style function
     */
    template<typename EventType>
    void Bind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        auto &handlers = GetOrCreateEvtHandlers(typeid(EventType));
        handlers.push_back((Event::EventFunctionWrapper(c_func)));
    }
    
    /**
     * @brief Bind event handler for EventType 
     * @description This overload binds event handler functor that is wrapped in std::function
     * Warning: it's not possible to unbind event std::function is temporary variable
     * @param[in] functor functor wrapped in std::function
     */
    template<typename EventType>
    void Bind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        auto &handlers = GetOrCreateEvtHandlers(typeid(EventType));
        handlers.push_back((Event::EventFunctionWrapper(functor)));
    }

    /**
     * @brief Bind event handler for EventType 
     * @description This overload binds handler member function and class instance
     * Warning: EventHandler parent can outlive member. In this case UB occured
     * @param[in] functor functor wrapped in std::function
     */
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
    /**
     * @brief Unbind event handler for EventType
     * @description This overload unbinds event handler c-style function
     * @throws throws error if function is not bind
     * @param[in] c_func c-style function
     */
    template<typename EventType>
    void Unbind(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))    
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char*>(c_func), 0 });
    }
    
    /**
     * @brief Unbind event handler for EventType
     * @description This overload unbinds event handler functor that is wrapped in std::function.
     * DON"T unbind lambda directly, instead unbind std::function that is bind to EventType and contains lambda/c_func
     * @throws throws error std::function is not bind
     * @param[in] functor that is wrapped in std::function 
     */
    template<typename EventType>
    void Unbind(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char *>(&functor), 0 } );
    }

    /**
     * @brief Unbind event handler for EventType
     * @description This overload unbinds event handler member function and class instance
     * @throws throws error if member function and class instance are not bind
     * @param[in] memberFunction ClassType::MemberFunction
     * @param[in] member Instance of ClassType
     */
    template<typename EventType, typename ClassType>
    void Unbind(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
    {
        Unbind(typeid(EventType), Event::EventFunctionWrapper::FunctionParams{ reinterpret_cast<const char *>(memberFunction), reinterpret_cast<const char *>(member) });
    }

    /**
     * @brief Raises event that is derived from Event
     * @description Raises event if EventHandler is not sealed
     * and has Handler to handle this EventType
     * Ignores all upcoming events if EventHandler is sealed
     * @param[in] event Event object
     */
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
    
    /**
     * @brief Gets object that owns this EventHandler and matches interface IEventHandlable
     * @return IEventHandlable
     */
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
