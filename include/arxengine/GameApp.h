#ifndef ARX_GAMEAPP_H
#define ARX_GAMEAPP_H

#include "arxengine/ArxDefines.h"
#include <memory>
#include <queue>
#include "arxengine/evt/Event.h"
#include <set>
#include <memory>

#ifdef WIN32
#include <windows.h>
#define MAIN_ENTRY_POINT int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
#else
#define MAIN_ENTRY_POINT int main(int, const char **)
#endif

ARX_NAMESPACE_BEGIN

class EventProcessor;
class SoundDevice;
class SoundContext;
class FontLoader;
class ArxWindow;

class ARX_EXPORTS DeleteEvent : public Event
{
private:
    friend class ArxObject;
    DeleteEvent() = default;
    void HandleEvent() override;
};

using ArxWindowSet = std::set<ArxWindow*>;

class ARX_EXPORTS GameApp
{
private:
    friend class DeleteEvent;
public:
    static int SetAppAsGlobal(std::unique_ptr<GameApp> &&app);
    static GameApp *GetGlobalApp();
    GameApp();
    int Init();
    bool IsInitialized() const;
    
    //must be created at least one window before or in loop
    //otherwise loop will continues forever until exit is called
    int Run();

    //all existed windows will closed and deleted 
    int Exit(int code);
   
    SoundContext &GetDefaultSoundDeviceContext();
    FontLoader &GetFontLoader();
    EventProcessor &GetEventProcessor();
    const ArxWindowSet &GetWindowSet();
    virtual ~GameApp();

    //delta time is calculated in Run()
    double GetDeltaTime() const;
protected:
    virtual void OnRun() {};
    virtual void OnAfterInit() {};
    virtual void CleanUp();
private:
    bool m_initialized;
    bool m_running;
    bool m_shouldExit;
    int m_exitCode;
    double m_deltaTime;
    ArxWindowSet m_windows;
    std::queue<ArxObject*> m_deleteQueue;
    std::unique_ptr<EventProcessor> m_eventProcessor;
    SoundDevice *m_soundDevice;
    SoundContext *m_soundContext;
    std::unique_ptr<FontLoader> m_fontLoader;
};


#define GlobalGameApp ::ARX_NAMESPACE::GameApp::GetGlobalApp()

#define IMPLEMENT_GAMEAPP_NO_MAIN(App, errCodeVariableName)\
    ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init()

#define IMPLEMENT_GAMEAPP_NO_MAIN_WITH_LOGGER(App, errCodeVariableName, LoggerClass)\
    ::ARX_NAMESPACE::Logger::SetGlobalLogger(std::make_unique<LoggerClass>());\
    ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init()

#define IMPLEMENT_GAMEAPP_NO_MAIN_WITH_LOGGER_INSTANCE(App, errCodeVariableName, LoggerInstanceUniquePtr)\
    ::ARX_NAMESPACE::Logger::SetGlobalLogger(LoggerInstanceUniquePtr);\
    ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init()

#define IMPLEMENT_GAMEAPP(App) MAIN_ENTRY_POINT { \
    int errCode = 0;\
    try\
    {\
        ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
        errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init();\
        if (errCode == static_cast<int>(::ARX_NAMESPACE::ArxException::ErrorCode::NoError))\
            errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Run();\
    }\
    catch(...)\
    {\
        errCode = static_cast<int>(::ARX_NAMESPACE::ArxException::ErrorCode::GenericError);\
    }\
    return errCode;\
}

#define IMPLEMENT_GAMEAPP_WITH_LOGGER(App, LoggerInstance) MAIN_ENTRY_POINT { \
    int errCode = 0;\
    try\
    {\
        ::ARX_NAMESPACE::Logger::SetGlobalLogger(LoggerInstance); \
        ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
        errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init();\
        if (errCode == static_cast<int>(::ARX_NAMESPACE::ArxException::ErrorCode::NoError))\
            errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Run();\
    }\
    catch(...)\
    {\
        errCode = static_cast<int>(::ARX_NAMESPACE::ArxException::ErrorCode::GenericError);\
    }\
    return errCode;\
}

ARX_NAMESPACE_END

#endif
