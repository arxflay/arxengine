#include "EventProcessor.h"
#include "internal/UniversalExceptionHandler.h"
#include "Event.h"
#include "EventManager.h"

ARX_NAMESPACE_BEGIN

void EventProcessor::ProcessEvents()
{
    while(!m_eventQueue.empty() && !m_stopEventProcessing)
    {
        try
        {
            std::unique_ptr<Event> evt = std::move(m_eventQueue.front());
            m_eventQueue.pop();

            CallBeforeProcessing(*evt.get());
            if (!evt->m_eventHandlersPtr.has_value())
            {
                evt->HandleEvent();
                continue;
            }

            bool shouldContinueProcessing = true;
            evt->Skip(false);
            typename std::vector<int>::difference_type evtHandlerIndex = 0;
        
            while(shouldContinueProcessing && !m_stopEventProcessing)
            {
                if(evtHandlerIndex < static_cast<decltype(evtHandlerIndex)>(evt->m_eventHandlersPtr->get().size()))
                {
                    auto handler = *(evt->m_eventHandlersPtr->get().rbegin() + evtHandlerIndex);
                    handler(*evt);
                    shouldContinueProcessing = evt->WasSkipCalled();
                    evtHandlerIndex++;
                    evt->Skip(false);
                }
                else
                {
                    evt->HandleEvent();
                    shouldContinueProcessing = false; //ignore Skip to avoid loop
                }
            }
        }
        catch(...)
        {
            /*do nothing*/
            UniversalExceptionHandler::HandleException();
        }
    }
    m_stopEventProcessing = false;
}

void EventProcessor::EnqueueEvent(std::unique_ptr<Event> &&event)
{
    m_eventQueue.emplace(std::move(event));
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
