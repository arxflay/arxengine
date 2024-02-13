#pragma once
#include "ArxDefines.h"
#include "ArxObject.h"
#include <type_traits>
#include <functional>
#include <optional>
#include <memory>

ARX_NAMESPACE_BEGIN

class Event;

template<typename EventType>
struct ARX_EXPORTS is_event_type
{
    static const bool value = std::is_base_of_v<Event, std::decay_t<EventType>>;
};

template<typename EventType>
static inline constexpr bool is_event_type_v = is_event_type<EventType>::value;

class EventHandler
{
public:
    struct UnwrappedFuncAddrs
    {
        constexpr bool operator==(const UnwrappedFuncAddrs &b) const
        {
            return addr == b.addr && instance_addr == b.instance_addr; 
        }
        const char *addr;
        const char *instance_addr;
    };

    template<typename EventType>
    constexpr EventHandler(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))
    {
        m_func = [c_func](Event& evt) constexpr -> void { c_func(static_cast<EventType&>(evt)); };
        m_originalFuncAddress = UnwrappedFuncAddrs{ reinterpret_cast<const char *>(c_func), nullptr };
    }

    template<typename EventType>
    constexpr EventHandler(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor) //fix
    {
        m_func = [functor](Event& evt) constexpr -> void { functor(static_cast<EventType&>(evt)); };
        m_originalFuncAddress = UnwrappedFuncAddrs { reinterpret_cast<const char*>(&functor), nullptr };
    }

    template<typename EventType, typename ClassType>
    constexpr EventHandler(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *instance)
    {
        m_func = [instance, memberFunction](Event& evt) constexpr -> void { (instance.*memberFunction)(static_cast<EventType&>(evt)); };
        m_originalFuncAddress = UnwrappedFuncAddrs{ reinterpret_cast<const char*>(memberFunction),reinterpret_cast<const char*>(instance) };
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

using EventHandlersVector = std::vector<EventHandler>;
using EventHandlersVectorRef = std::reference_wrapper<EventHandlersVector>;
using EventHandlersVectorCref = std::reference_wrapper<const EventHandlersVector>; 

class Event : public ArxObject
{
public:
    friend void ProcessEvent(Event &evt, const bool &shouldExit);
    friend class EventManager;
    Event();
    Event(ArxObject *sender);

    void Skip(bool skip = true);
    bool WasSkipCalled(); 
    ArxObject *GetSender();
    void SetSender(ArxObject *sender);
private:
    //DO NOT CALL DIRECTLY
    virtual void HandleEvent() = 0; //default event handler
    bool m_skipped;
    std::optional<EventHandlersVectorRef> m_eventHandlersPtr;
    ArxObject *m_sender;
};

ARX_NAMESPACE_END
