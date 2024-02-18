#pragma once 
#include "ArxDefines.h"
#include <memory>
#include <queue>
#include "Event.h"
#include "internal/UniversalExceptionHandler.h"
#include <set>

ARX_NAMESPACE_BEGIN

class EventProcessor;
class SoundDeviceContextPair;
class FontLibrary;
class ArxWindow;

class DeleteEvent : public Event
{
private:
    void HandleEvent() override;
};

using ArxWindowSet = std::set<ArxWindow*>;

class GameApp
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
   
    SoundDeviceContextPair &GetSoundDeviceContextPair();
    FontLibrary &GetFontLibrary();
    EventProcessor &GetEventProcessor();
    const ArxWindowSet &GetWindowSet();
    virtual ~GameApp();
protected:
    virtual void OnRun() {};
    virtual void OnAfterInit() {};
    virtual void CleanUp();
private:
    bool m_initialized;
    bool m_running;
    bool m_shouldExit;
    int m_exitCode;
    ArxWindowSet m_windows;
    std::queue<ArxObject*> m_deleteQueue;
    std::unique_ptr<EventProcessor> m_eventProcessor;
    std::unique_ptr<SoundDeviceContextPair, void(*)(SoundDeviceContextPair*)> m_soundDeviceContextPair;
    std::unique_ptr<FontLibrary, void(*)(FontLibrary*)> m_fontLibrary;
};


#ifdef _WIN32
#include <winbase.h>
#define MAIN_ENTRY_POINT int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
#else
#define MAIN_ENTRY_POINT int main(int, const char **)
#endif

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
        errCode = static_cast<int>(::ARX_NAMESPACE::UniversalExceptionHandler::HandleException());\
    }\
    return errCode;\
}

#define IMPLEMENT_GAMEAPP_WITH_LOGGER(App, LoggerClass) MAIN_ENTRY_POINT { \
    int errCode = 0;\
    try\
    {\
        ::ARX_NAMESPACE::Logger::SetGlobalLogger(std::make_unique<LoggerClass>()); \
        ::ARX_NAMESPACE::GameApp::SetAppAsGlobal(std::make_unique<App>());\
        errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Init();\
        if (errCode == static_cast<int>(::ARX_NAMESPACE::ArxException::ErrorCode::NoError))\
            errCode = ::ARX_NAMESPACE::GameApp::GetGlobalApp()->Run();\
    }\
    catch(...)\
    {\
        errCode = static_cast<int>(::ARX_NAMESPACE::UniversalExceptionHandler::HandleException());\
    }\
    return errCode;\
}

ARX_NAMESPACE_END
