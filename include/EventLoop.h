#pragma once
#include "arxdefines.h"
#include "Event.h"

ARX_NAMESPACE_BEGIN

class ExitRequestEvent : public Event
{
public:
    ExitRequestEvent();
    ExitRequestEvent(int code);
    constexpr int GetCode() const { return m_code; }
private:
    int m_code;
};

class ARX_EXPORTS EventLoop
{
public:
    EventLoop();
    int Run();
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> EnqueueEvent(EventType &&evt)
    {
        m_eventQueue.push(std::forward<EventType>(evt));
    }
    constexpr bool IsRunning() const { return m_running; };
    void Exit(int code = 0);
private:
    virtual void GenerateEvents();
    void PollEvents();
    bool m_running;
    bool m_shouldExit;
    int m_exitCode;

    std::queue<Event> m_eventQueue;
};

ARX_NAMESPACE_END
