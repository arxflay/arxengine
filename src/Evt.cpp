#include "Evt.h"
#include "ArxWindow.h"
#include "EventHandler.h"

#include <GLFW/glfw3.h>

ARX_NAMESPACE_BEGIN

void EventProcessor::Process(Event &&e)
{
    while(true)
    {
        e.Process();
        if(!e.IsSkipCalled())
            break;
        e.SetNextEvtFunc();
        e.Skip(false);
    }
}

Event::EvtFuncWrapperIterator::EvtFuncWrapperIterator(EvtFuncWrapperInnerIterator begin, EvtFuncWrapperInnerIterator end)
        : m_current(begin), m_end(end)
{
}

Event::EvtFuncWrapperIterator Event::EvtFuncWrapperIterator::Next()
{
    EvtFuncWrapperIterator next;
    assert(HasNext());
    next.m_current = ++m_current;
    next.m_end = m_end;
    return next;
}

bool Event::EvtFuncWrapperIterator::HasNext()
{
    return m_current + 1 != m_end && m_current != m_end;
}

std::reference_wrapper<Event::EventFunctionWrapper> Event::EvtFuncWrapperIterator::GetValue()
{
    assert(m_current != m_end);
    return std::ref(*m_current); 
}

Event::Event()
    : m_callingObject(nullptr), m_skipCalled(false)
{
}

void Event::Process()
{
    m_evtFuncIterator.GetValue()(*this);
}
    
IEventHandlable *Event::GetCallingObject()
{
    return m_callingObject;
}

void Event::Skip(bool skip)
{
    m_skipCalled = skip;
}

void Event::SetEvtFuncWrapperIterator(EvtFuncWrapperIterator iterator)
{
    m_evtFuncIterator = iterator;
}

void Event::SetNextEvtFunc()
{
    m_evtFuncIterator = m_evtFuncIterator.Next();
}

void Event::SetObject(IEventHandlable *callingObject) 
{
    m_callingObject = callingObject;
}

bool Event::IsSkipCalled()
{
    return m_skipCalled;
}

int EventLoop::Run()
{
    EventProcessor evtProcessor;
    while(true)
    {
        glfwPollEvents();
        while(!m_eventQueue.empty())
        {
            evtProcessor.Process(std::move(m_eventQueue.front()));
            m_eventQueue.pop();
        }

        ArxWindow *win = static_cast<ArxWindow*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
        win->Draw();
    }
    return 0;
}


ARX_NAMESPACE_END
