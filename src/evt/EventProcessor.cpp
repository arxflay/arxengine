#include "arxengine/evt/EventProcessor.h"
#include "arxengine/UniversalExceptionHandler.h"
#include "arxengine/evt/Event.h"
#include "arxengine/evt/EventManager.h"
#include "arxengine/ArxObject.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

EventProcessor::EventProcessor()
    : m_stopEventProcessing(false)
    , m_eventsProcessingRunning(false)
{

}

void EventProcessor::ProcessEventInternal(Event &event)
{
    CallBeforeProcessing(event);
    auto eventHandlersOpt = event.m_getEventHandlersFn();
    if (!eventHandlersOpt.has_value())
    {
        event.HandleEvent();
        return;
    }

    auto &eventHandlersRef = eventHandlersOpt.value().get();
    bool hasMoreHandlers = true;
    event.Skip(false);
    typename std::vector<int>::difference_type evtHandlerIndex = 0;

    while(hasMoreHandlers && !IsInterruptCalled())
    {
        if(evtHandlerIndex < static_cast<decltype(evtHandlerIndex)>(eventHandlersRef.size()))
        {
            auto handler = *(eventHandlersRef.rbegin() + evtHandlerIndex);
            handler(event);
            hasMoreHandlers = event.WasSkipCalled();
            evtHandlerIndex++;
        }
        else
        {
            event.HandleEvent();
            hasMoreHandlers = false;
        }
        event.Skip(false);
    }

}

void EventProcessor::ProcessEvent(Event &event)
{

    bool oldProcessingState = IsInterruptCalled();
    m_stopEventProcessing = false;
    try
    {
        ProcessEventInternal(event);
    }
    catch(...)
    {
        /*do nothing*/
        UniversalExceptionHandler::HandleException();
    }
    m_stopEventProcessing = oldProcessingState;
}

bool EventProcessor::IsEventsProcessingRunning() const
{
    return m_eventsProcessingRunning ;
}

bool EventProcessor::ShouldStopProcessingEvents() const
{
    return m_eventQueue.empty() || IsInterruptCalled();
}

bool EventProcessor::IsInterruptCalled() const
{
    return m_stopEventProcessing;
}

void EventProcessor::ProcessEvents()
{
    if (IsEventsProcessingRunning())
        return;
    
    m_eventsProcessingRunning = true;
    m_stopEventProcessing = false;
    
    while(!ShouldStopProcessingEvents())
    {
        try
        {
            std::unique_ptr<Event> event = std::move(m_eventQueue.front());
            m_eventQueue.pop();
            if (!event)
                throw ArxException(ArxException::ErrorCode::GenericError, "nullptr event arrived");
         
            ProcessEventInternal(*event);
            if (event->IsScheduledAfterProcessing())
            {
                event->ScheduleAfterProcessing(false);
                m_scheduledEvents.push(std::move(event));
            }
        }
        catch(...)
        {
            /*do nothing*/
            UniversalExceptionHandler::HandleException();
        }
    }
    
    MoveScheduledEventsToEventQueue();
    m_eventsProcessingRunning = false;
}

void EventProcessor::MoveScheduledEventsToEventQueue()
{
    try
    {
        while(!m_scheduledEvents.empty())
        {
            std::unique_ptr<Event> evt = std::move(m_scheduledEvents.front());
            if (!(evt->GetSender() && evt->GetSender()->IsDestroyCalled()))
                m_eventQueue.emplace(std::move(evt));

            m_scheduledEvents.pop();
        }
    }
    catch(...)
    {
        /*do nothing*/
        UniversalExceptionHandler::HandleException();
    }
}

void EventProcessor::EnqueueEvent(std::unique_ptr<Event> &&event)
{
    m_eventQueue.emplace(std::move(event));
}

void EventProcessor::ScheduleEvent(std::unique_ptr<Event> &&event)
{
    m_scheduledEvents.emplace(std::move(event));
}

void EventProcessor::RemoveScheduledEvents()
{
    m_scheduledEvents = {};
}

void EventProcessor::RemoveAllEvents()
{
    RemoveEvents();
    RemoveScheduledEvents();
}

void EventProcessor::Interrupt()
{
    m_stopEventProcessing = true;
}

void EventProcessor::RemoveEvents()
{
    m_eventQueue = {};
}

ARX_NAMESPACE_END
