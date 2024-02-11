#pragma once 
#include "ArxDefines.h"
#include <ArxException.h>
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

#define IMPLEMENT_UIAPP_NO_MAIN(App)\
    UIApp::SetAppAsGlobal(std::make_unique<App>());\
    UIApp::GetGlobalApp()->Init()

#define IMPLEMENT_UIAPP(App) MAIN_ENTRY_POINT { \
    IMPLEMENT_UIAPP_NO_MAIN(App);\
    return UIApp::GetGlobalApp()->Run();\
}

ARX_NAMESPACE_END
