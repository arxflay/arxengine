#ifndef ARX_EVENT_H
#define ARX_EVENT_H

#include "arxengine/ArxDefines.h"
#include <type_traits>
#include <functional>
#include <optional>
#include <memory>
#include <vector>

ARX_NAMESPACE_BEGIN

class Event;

template<typename EventType>
struct is_event_type
{
    static const bool value = std::is_base_of_v<Event, std::decay_t<EventType>>;
};

template<typename EventType>
static inline constexpr bool is_event_type_v = is_event_type<EventType>::value;

namespace detail
{
    class EventHandler;
}

using EventHandlersVector = std::vector<detail::EventHandler>;
using EventHandlersVectorRef = std::reference_wrapper<EventHandlersVector>;
using EventHandlersVectorCref = std::reference_wrapper<const EventHandlersVector>;

class ArxObject;
class ARX_EXPORTS Event 
{
public:
    friend class EventProcessor; 
    friend class EventManager;
    Event();

    void Skip(bool skip = true);
    bool WasSkipCalled(); 
    ArxObject *GetSender();
    void ScheduleAfterProcessing(bool schedule);
    bool IsScheduledAfterProcessing() const; //TODO
    virtual ~Event() = default;
private:
    void SetSender(ArxObject *sender);
    virtual void HandleEvent() = 0; //default event handler
    bool m_skipped;
    bool m_scheduleAfterProcessing; 
    std::function<std::optional<EventHandlersVectorRef>()> m_getEventHandlersFn;
    ArxObject *m_sender;
};

ARX_NAMESPACE_END

#endif
