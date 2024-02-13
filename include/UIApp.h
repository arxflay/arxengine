#pragma once 
#include "ArxDefines.h"
#include "ArxException.h"
#include <memory>

ARX_NAMESPACE_BEGIN

class UIEventLoop;
class SoundDeviceContextPair;
class FontLibrary;

class UIApp
{
public:
    static int SetAppAsGlobal(std::unique_ptr<UIApp> &&app);
    static UIApp *GetGlobalApp();
    UIApp();
    int Init();
    bool IsInitialized() const;
    int Run(); //TODO
   
    SoundDeviceContextPair &GetSoundDeviceContextPair();
    FontLibrary &GetFontLibrary();
    UIEventLoop &GetEventLoop(); //TODO
    virtual ~UIApp();
protected:
    virtual void OnRun() {};
    virtual void OnAfterInit() {};
private:
    bool m_initialized;
    std::unique_ptr<UIEventLoop> m_eventLoop;
    std::unique_ptr<SoundDeviceContextPair, void(*)(SoundDeviceContextPair*)> m_soundDeviceContextPair;
    std::unique_ptr<FontLibrary, void(*)(FontLibrary*)> m_fontLibrary;
};


#ifdef _WIN32
#include <winbase.h>
#define MAIN_ENTRY_POINT int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
#else
#define MAIN_ENTRY_POINT int main(int, const char **)
#endif

#define IMPLEMENT_UIAPP_NO_MAIN(App, errCodeVariableName)\
    ARX_NAMESPACE::UIApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ARX_NAMESPACE::UIApp::GetGlobalApp()->Init()

#define IMPLEMENT_UIAPP_NO_MAIN_WITH_LOGGER(App, errCodeVariableName, LoggerClass)\
    ARX_NAMESPACE::Logger::SetGlobalLogger(std::make_unique<LoggerClass>());\
    ARX_NAMESPACE::UIApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ARX_NAMESPACE::UIApp::GetGlobalApp()->Init()

#define IMPLEMENT_UIAPP_NO_MAIN_WITH_LOGGER_INSTANCE(App, errCodeVariableName, LoggerInstanceUniquePtr)\
    ARX_NAMESPACE::Logger::SetGlobalLogger(LoggerInstanceUniquePtr);\
    ARX_NAMESPACE::UIApp::SetAppAsGlobal(std::make_unique<App>());\
    int errCodeVariableName = ARX_NAMESPACE::UIApp::GetGlobalApp()->Init()
    

#define IMPLEMENT_UIAPP(App) MAIN_ENTRY_POINT { \
    ARX_NAMESPACE::UIApp::SetAppAsGlobal(std::make_unique<App>());\
    int err = ARX_NAMESPACE::UIApp::GetGlobalApp()->Init();\
    if (err != static_cast<int>(ARX_NAMESPACE::ArxException::ErrorCode::NoError))\
        return err;\
    return ARX_NAMESPACE::UIApp::GetGlobalApp()->Run();\
}

#define IMPLEMENT_UIAPP_WITH_LOGGER(App, LoggerClass) MAIN_ENTRY_POINT { \
    ARX_NAMESPACE::Logger::SetGlobalLogger(std::make_unique<LoggerClass>()); \
    ARX_NAMESPACE::UIApp::SetAppAsGlobal(std::make_unique<App>());\
    int err = ARX_NAMESPACE::UIApp::GetGlobalApp()->Init();\
    if (err != static_cast<int>(ARX_NAMESPACE::ArxException::ErrorCode::NoError))\
        return err;\
    return ARX_NAMESPACE::UIApp::GetGlobalApp()->Run();\
}

ARX_NAMESPACE_END
