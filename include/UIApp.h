#pragma once
#include "arxdefines.h"
#include "EventLoop.h"

ARX_NAMESPACE_BEGIN

class ArxWindow;

class ARX_EXPORTS UIApp
{
private:
    class ARX_EXPORTS TopLevelWindowManager 
    {
        friend class ArxWindow;
    protected:
        void AddTopLevelWindow(ArxWindow *win);
        void RemoveTopLevelWindow(ArxWindow *win);
        bool HasTopLevelWindows();
        void Reset();
    private:
        std::vector<ArxWindow*> m_topLevelWindows;
        using TopLevelWindowIterator = decltype(m_topLevelWindows)::iterator;
        TopLevelWindowIterator FindWindow(ArxWindow *win);
    };

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
    TopLevelWindowManager &GetTopLevelWindowManager();

    virtual void OnExec();
    int Exec();
    virtual ~UIApp();
private:
    EventLoop m_evtLoop;
    TopLevelWindowManager m_topLevelWinManager;
    bool InitGlfw();
    bool InitGlad();
    bool m_initialized;
};

#define ArxTheApp (UIApp::GetInstance())

ARX_NAMESPACE_END
