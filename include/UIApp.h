#pragma once 
#include "ArxDefines.h"
#include <memory>

struct ALCdevice;
struct ALCcontext;

ARX_NAMESPACE_BEGIN

class UIApp
{
public:
    static int SetAppAsGlobal(std::unique_ptr<UIApp> &&app);
    static UIApp *GetGlobalApp();
    UIApp();
    int Init();
    bool IsInitialized() const;
private:
    void InitAL();
    bool m_initialized;
    ALCdevice *m_device;
    ALCcontext *m_context;
};


ARX_NAMESPACE_END
