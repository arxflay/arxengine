#pragma once 
#include "ArxDefines.h"
#include <ArxException.h>
#include <memory>

ARX_NAMESPACE_BEGIN

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
   
    SoundDeviceContextPair &GetSoundDeviceContextPair();
    FontLibrary &GetFontLibrary();
    virtual ~UIApp();

private:
    bool m_initialized;
    std::unique_ptr<SoundDeviceContextPair, void(*)(SoundDeviceContextPair*)> m_soundDeviceContextPair;
    std::unique_ptr<FontLibrary, void(*)(FontLibrary*)> m_fontLibrary;
};

ARX_NAMESPACE_END
