#include "EventLoop.h"
#include "ArxWindow.h"
#include <GLFW/glfw3.h>

ARX_NAMESPACE_BEGIN

int EventLoop::Run()
{
    EventProcessor evtProcessor;
    while(true)
    {
        PollEvents();
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

void EventLoop::PollEvents()
{
    glfwPollEvents();
}


ARX_NAMESPACE_END
