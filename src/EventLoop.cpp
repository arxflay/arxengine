#include "EventLoop.h"
#include "ArxWindow.h"
#include <GLFW/glfw3.h>

ARX_NAMESPACE_BEGIN

ExitRequestEvent::ExitRequestEvent()
    : m_code(0)
{
}

ExitRequestEvent::ExitRequestEvent(int code)
    : m_code(code)
{
}

template<typename T>
class VariableRestoreGuard
{
public:
    VariableRestoreGuard(T *variable, T oldValue)
        : m_varaible(variable), m_oldValue(oldValue)
    {

    }

    ~VariableRestoreGuard()
    {
        if (m_varaible)
            *m_varaible = m_oldValue;
    }
private:
    T *m_varaible;
    T m_oldValue;
};

EventLoop::EventLoop()
    : m_running(false), m_shouldExit(false)
{
}

int EventLoop::Run()
{
    if (m_running)
        return 0;
    
    m_running = true;
    VariableRestoreGuard<bool> guard(&m_running, false);
    VariableRestoreGuard<bool> shouldExitGuard(&m_shouldExit, false);
    EventProcessor evtProcessor;
    while(!m_shouldExit)
    {
        PollEvents();
        while(!m_eventQueue.empty() && !m_shouldExit)
        {
            evtProcessor.Process(std::move(m_eventQueue.front()));
            m_eventQueue.pop();
        }

        if (m_shouldExit)
            break;

        ArxWindow *win = static_cast<ArxWindow*>(glfwGetWindowUserPointer(glfwGetCurrentContext()));
        win->Draw();
    }
    return m_exitCode;
}

void EventLoop::PollEvents()
{
    glfwPollEvents();
}

void EventLoop::Exit(int code)
{
    m_exitCode = code;
    m_shouldExit = true;
}

void EventLoop::GenerateEvents()
{
}


ARX_NAMESPACE_END
