#pragma once
#include "arxdefines.h"

#include <atomic>
#include <memory>
#include <shared_mutex>
#include <mutex>
#include <functional>
#include <cassert>
#include <queue>

ARX_NAMESPACE_BEGIN

class Event;
class EventHandler;

template<typename EventType>
struct ARX_EXPORTS is_event_type
{
    static const bool value = std::is_base_of_v<Event, std::decay_t<EventType>>;
};

template<typename EventType>
static inline constexpr bool is_event_type_v = is_event_type<EventType>::value;

class ARX_EXPORTS IEventHandlable
{
public:
    virtual ~IEventHandlable() = default;
protected:
    virtual EventHandler &GetEventHandler() noexcept = 0;
    virtual const EventHandler &GetEventHandler() const noexcept = 0;
};

class ARX_EXPORTS EventProcessor final
{
public:
    void Process(Event &&e);
};

class ARX_EXPORTS Event
{
friend class EventHandler;
friend class EventProcessor;
private:
    class ARX_EXPORTS EventFunctionWrapper
    {
    public:
        struct ARX_EXPORTS FunctionParams
        {
            bool operator==(const FunctionParams &b) const
            {
                return addr == b.addr && member_addr == b.member_addr; 
            }
            const char *addr;
            const char *member_addr;
        };

        template<typename EventType>
        EventFunctionWrapper(std::enable_if_t<is_event_type_v<EventType>> (*c_func)(EventType&))
        {
            m_func = [c_func](Event& evt) -> void { c_func(static_cast<EventType&>(evt)); };
            m_getOriginalFuncParams = [c_func]() -> FunctionParams { return FunctionParams{ reinterpret_cast<const char *>(c_func), nullptr };};
        }

        template<typename EventType>
        EventFunctionWrapper(const std::function<std::enable_if_t<is_event_type_v<EventType>>(EventType&)> &functor)
        {
            m_func = [functor](Event& evt) -> void { functor(static_cast<EventType&>(evt)); };
            const char *addr = reinterpret_cast<const char *>(&functor);
            m_getOriginalFuncParams = [addr]() -> FunctionParams { return FunctionParams{ addr, nullptr };};
        }

        template<typename EventType, typename ClassType>
        EventFunctionWrapper(std::enable_if_t<is_event_type_v<EventType>> (ClassType::*memberFunction)(EventType&), ClassType *member)
        {
            m_func = [member, memberFunction](Event& evt) -> void { std::invoke(member, memberFunction, static_cast<EventType&>(evt)); };
            m_getOriginalFuncParams = [member, memberFunction]() -> FunctionParams { return FunctionParams{ reinterpret_cast<const char*>(memberFunction),reinterpret_cast<const char*>(member) };};
        }

        void operator()(Event&evt) const
        {
            m_func(evt);
        }

        bool operator==(const EventFunctionWrapper &wrapper) const
        {
            return GetOriginalFuncParams() == wrapper.GetOriginalFuncParams();
        }

        FunctionParams GetOriginalFuncParams() const
        {
            return m_getOriginalFuncParams();
        }

    private:
        std::function<FunctionParams()> m_getOriginalFuncParams;
        std::function<void(Event&)> m_func;
    };
    
    using EventFunctionWrapperVec = std::vector<EventFunctionWrapper>;
    class ARX_EXPORTS EvtFuncWrapperIterator
    {
    private:
        using EvtFuncWrapperInnerIterator = EventFunctionWrapperVec::reverse_iterator;

    public:
        EvtFuncWrapperIterator() = default;
        EvtFuncWrapperIterator(EvtFuncWrapperInnerIterator begin, EvtFuncWrapperInnerIterator end);
        EvtFuncWrapperIterator Next();
        bool HasNext();
        std::reference_wrapper<EventFunctionWrapper> GetValue();
    private:
        EvtFuncWrapperInnerIterator m_current;
        EvtFuncWrapperInnerIterator m_end;
    };

public:
    Event();
    IEventHandlable *GetCallingObject();
    void Skip(bool skip = true);
protected:
    bool IsSkipCalled();
    void SetObject(IEventHandlable *callingObject); 
private:
    void Process();
    void SetEvtFuncWrapperIterator(EvtFuncWrapperIterator iterator);
    void SetNextEvtFunc();
    IEventHandlable *m_callingObject;
    EvtFuncWrapperIterator m_evtFuncIterator;
    bool m_skipCalled;
};



ARX_NAMESPACE_END


