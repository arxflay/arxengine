#pragma once
#include "arxdefines.h"
#include "Event.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS EventLoop
{
public:
    int Run();
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> EnqueueEvent(EventType &&evt)
    {
        m_eventQueue.push(std::forward<EventType>(evt));
    }
private:
    void PollEvents();
    std::queue<Event> m_eventQueue;
};

ARX_NAMESPACE_END
