#pragma once
#include "arxdefines.h"
#include "EventLoop.h"

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS UIApp
{
public:
    static void SetInstance(UIApp *app);
    static UIApp *GetInstance();
    UIApp();
    int Init();
    
    template<typename EventType>
    std::enable_if_t<is_event_type_v<EventType>> EnqueueEvent(EventType &&evt)
    {
        m_evtLoop.EnqueueEvent(std::forward<EventType>(evt));
    }
    
    EventLoop &GetEventLoop();
    virtual void OnExec();
    int Exec();
    virtual ~UIApp();
private:
    EventLoop m_evtLoop;
    bool InitGlfw();
    bool InitGlad();
    bool m_initialized;
};

ARX_NAMESPACE_END
