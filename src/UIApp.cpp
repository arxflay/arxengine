#include "UIApp.h"
#include <AL/alc.h>
ARX_NAMESPACE_BEGIN

std::unique_ptr<UIApp> g_app;

/*static*/ int UIApp::SetAppAsGlobal(std::unique_ptr<UIApp> &&app)
{
    if (GetGlobalApp() != nullptr && GetGlobalApp()->IsInitialized())
        return -2; //already initialized;

    g_app = std::move(app);
    return 0;
}

/*static*/ UIApp* UIApp::GetGlobalApp()
{
    return g_app.get();
}

void UIApp::InitAL()
{
    m_device = alcOpenDevice(nullptr);
    m_context = alcCreateContext(m_device, nullptr);
    alcMakeContextCurrent(m_context);
}

UIApp::UIApp()
    : m_initialized(false), m_device(nullptr), m_context(nullptr)
{

}

int UIApp::Init()
{
    if (GetGlobalApp() != this)
        return -1; //cant initialize if app is not set as global

    InitAL();
    m_initialized = true;
    return 0;
}

bool UIApp::IsInitialized() const
{
    return m_initialized;
}

ARX_NAMESPACE_END
