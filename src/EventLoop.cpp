#include "EventLoop.h"
#include "internal/UniversalExceptionHandler.h"
#include "GLFW/glfw3.h"
ARX_NAMESPACE_BEGIN

//unfancy
void ProcessEvent(Event &evt, const bool &shouldExit)
{
    if (!evt.m_eventHandlersPtr.has_value())
    {
        evt.HandleEvent();
        return;
    }

    bool shouldContinueProcessing = false;
    typename std::vector<int>::difference_type evtHandlerIndex = 0;
    
    while(!shouldContinueProcessing && !shouldExit)
    {
        if(evtHandlerIndex < static_cast<decltype(evtHandlerIndex)>(evt.m_eventHandlersPtr->get().size()))
        {
            auto handler = *(evt.m_eventHandlersPtr->get().rbegin() + evtHandlerIndex);
            handler(evt);
            shouldContinueProcessing = evt.WasSkipCalled();
            evtHandlerIndex++;
            evt.Skip(false);
        }
        else
        {
            evt.HandleEvent();
            shouldContinueProcessing = true; //ignore Skip to avoid loop
        }
    }
}

int EventLoop::Run()
{
    if (m_running)
        return EventLoopErrorCode::EventLoopAlreadyRunning;
    
    m_running = true;
    while(!m_shouldExit)
    {
        try
        {
            CallBeforeLoopIteration();
            while(!m_eventQueue.empty() && !m_stopEventProcessing)
            {
                std::unique_ptr<Event> evt = std::move(m_eventQueue.front());
                CallBeforeProcessing(*evt.get());
                ProcessEvent(*evt.get(), m_stopEventProcessing);
            }
        }
        catch(...)
        {
            /*do nothing*/
            UniversalExceptionHandler::HandleException();
        }
    }
    int code = m_exitCode;
    CleanUp();

    return code;
}

int EventLoop::Exit(int code)
{
    if (m_stopEventProcessing)
        return EventLoopErrorCode::EventLoopExitAlreadyCalled;
    m_stopEventProcessing = true;

    return ScheduleExit(code);
}

int EventLoop::ScheduleExit(int code)
{
    if (m_shouldExit)
        return EventLoopErrorCode::EventLoopExitAlreadyCalled;
    m_shouldExit = true;
    m_exitCode = code;

    return EventLoopErrorCode::NoError;
}

void EventLoop::CleanUp()
{
    m_exitCode = 0;
    m_eventQueue = decltype(m_eventQueue){};
    m_running = false;
    m_shouldExit = false;
    m_stopEventProcessing = false;
}

void EventLoop::EnqueueEvent(std::unique_ptr<Event> &&event)
{
    m_eventQueue.emplace(std::move(event));
}

void UIEventLoop::CallBeforeLoopIteration()
{
    glfwPollEvents();
}

void UIEventLoop::CallBeforeProcessing(Event &)
{
       
}

ARX_NAMESPACE_END
