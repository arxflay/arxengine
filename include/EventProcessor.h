#pragma once
#include "ArxDefines.h"
#include <queue>
#include <memory>

ARX_NAMESPACE_BEGIN

class Event;
class ArxObject;

class EventProcessor 
{
friend class DeleteEvent;
public:
    EventProcessor() = default; 
    EventProcessor(EventProcessor&&) = delete;
    EventProcessor(const EventProcessor&) = delete;
    EventProcessor &operator=(EventProcessor&&) = delete;
    EventProcessor &operator=(const EventProcessor&) = delete;

    enum EventLoopErrorCode
    {
        NoError = 0,
        EventLoopAlreadyRunning = 1,
        EventLoopIsNotRunning = 2,
        EventLoopExitAlreadyCalled = 3
    };

    //ignores exceptions
    void ProcessEvents(); //TODO
    void EnqueueEvent(std::unique_ptr<Event> &&event);
    void ScheduleEvent(std::unique_ptr<Event> &&event);
    void Interrupt();
    void RemoveEvents();
    void RemoveScheduledEvents();
    void RemoveAllEvents();

    virtual ~EventProcessor() = default;

protected:
    virtual void CallBeforeProcessing(Event &evt) = 0;
private:
    void MoveScheduledEventsToEventQueue();
    std::queue<std::unique_ptr<Event>> m_eventQueue;
    std::queue<std::unique_ptr<Event>> m_scheduledEvents;
    bool m_stopEventProcessing;
};


ARX_NAMESPACE_END
