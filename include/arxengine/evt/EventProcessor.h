/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_EVENT_PROCESSOR
#define ARX_EVENT_PROCESSOR
#include "arxengine/ArxDefines.h"
#include <queue>
#include <memory>

ARX_NAMESPACE_BEGIN

class Event;
class ArxObject;

class ARX_EXPORTS EventProcessor 
{
friend class DeleteEvent;
public:
    EventProcessor(); 
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
    void ProcessEvent(Event &event);

    /*if ProcessEvents still processing events, then do nothing*/
    void ProcessEvents(); //TODO
    void EnqueueEvent(std::unique_ptr<Event> &&event);
    void ScheduleEvent(std::unique_ptr<Event> &&event);
    void Interrupt();
    void RemoveEvents();
    void RemoveScheduledEvents();
    void RemoveAllEvents();
    
    bool IsEventsProcessingRunning() const;

    virtual ~EventProcessor() = default;

protected:
    virtual void CallBeforeProcessing(Event &evt) = 0;
private:
    void ProcessEventInternal(Event &event);
    bool ShouldStopProcessingEvents() const;
    bool IsInterruptCalled() const;
    

    void MoveScheduledEventsToEventQueue();
    std::queue<std::unique_ptr<Event>> m_eventQueue;
    std::queue<std::unique_ptr<Event>> m_scheduledEvents;
    bool m_stopEventProcessing;
    bool m_eventsProcessingRunning;
};


ARX_NAMESPACE_END

#endif
