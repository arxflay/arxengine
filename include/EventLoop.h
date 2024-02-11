#pragma once
#include "ArxDefines.h"
#include <queue>
#include <memory>
#include "Event.h"

ARX_NAMESPACE_BEGIN

class EventLoop
{
public:
    enum EventLoopErrorCode
    {
        NoError = 0,
        EventLoopAlreadyRunning = 1,
        EventLoopIsNotRunning = 2,
        EventLoopExitAlreadyCalled = 3
    };
    int Run(); //TODO
    int Exit(int code); //TODO
    int ScheduleExit(int code); //TODO

    //void GenerateIdleEvent(); //TODO
    void CleanUp();

    void EnqueueEvent(std::unique_ptr<Event> &&event);

protected:
    virtual void CallBeforeLoopIteration() = 0;
    virtual void CallBeforeProcessing(Event &evt) = 0;

private:
    int m_exitCode;
    std::queue<std::unique_ptr<Event>> m_eventQueue;
    bool m_shouldExit;
    bool m_stopEventProcessing;
    bool m_running;
};

class UIEventLoop final : public EventLoop
{
public:
    void CallBeforeLoopIteration() override; //TODO
    void CallBeforeProcessing(Event &evt) override; //TODO
};

ARX_NAMESPACE_END
