#include "UIApp.h"
#include "internal/UniversalExceptionHandler.h"
#include "SoundPlayer.h"
#include "Font.h"
#include "logging/Logger.h"
#include "EventLoop.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

ARX_NAMESPACE_BEGIN

namespace internal
{
    SoundDeviceContextPair *InitAL()
    {
        GLOG->Info("Initializing OpenAL...");
        return CreateSoundDeviceContextPair("");
    }

    FontLibrary *InitFT()
    {
        GLOG->Info("Initializing Freetype...");
        return CreateFontLibrary();
    }

    void InitGlad()
    {
        GLOG->Info("Initializing Glad...");
        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> tempWin(glfwCreateWindow(1, 1, "", nullptr, nullptr), [](GLFWwindow *win) {
            glfwMakeContextCurrent(nullptr);
            glfwDestroyWindow(win);        
        });
        glfwMakeContextCurrent(tempWin.get());
    }

    void InitGLFW()
    {
        GLOG->Info("Initializing GLFW...");
        int err = glfwInit();
        if (err != GLFW_TRUE)
            throw ArxException(ArxException::ErrorCode::FailedToInitializeFreetype, Utils::Format("Failed to initialize glfw library, glfw_error_code='%d'", err));

        glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, defaults::GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, defaults::GL_VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
}

std::unique_ptr<UIApp> g_app;

/*static*/ int UIApp::SetAppAsGlobal(std::unique_ptr<UIApp> &&app)
{
    if (GetGlobalApp() != nullptr && GetGlobalApp()->IsInitialized())
        return static_cast<int>(ArxException::ErrorCode::UIAppAlreadyInitialized);

    g_app = std::move(app);
    return 0;
}

/*static*/ UIApp *UIApp::GetGlobalApp()
{
    return g_app.get();
}

UIApp::UIApp()
    : m_initialized(false)
    , m_soundDeviceContextPair(nullptr, DestroySoundDeviceContextPair)
    , m_fontLibrary(nullptr, DestroyFontLibrary)
{
}

int UIApp::Init()
{
    int code = static_cast<int>(ArxException::ErrorCode::NoError);
    try
    {
        GLOG->Info("Initialization begin");
        if (GetGlobalApp() != this)
            throw ArxException(ArxException::ErrorCode::UIAppNotSetAsGlobal, "This app is not set as global");
        else if (m_initialized)
            throw ArxException(ArxException::ErrorCode::UIAppAlreadyInitialized, "App is already inititalized");
    
        internal::InitGLFW();
        internal::InitGlad();
        m_soundDeviceContextPair.reset(internal::InitAL());
        m_fontLibrary.reset(internal::InitFT());
        m_eventLoop = std::make_unique<UIEventLoop>();
        OnAfterInit();
        m_initialized = true;
        GLOG->Info("Initialization done");
    }
    catch(...)
    {
        code = static_cast<int>(UniversalExceptionHandler::HandleException());
        GLOG->Info("Initialization failed");
        m_soundDeviceContextPair.reset();
        m_fontLibrary.reset();
        glfwTerminate();
    }

    return code;
}

int UIApp::Run()
{
    OnRun();
    return m_eventLoop->Run();
}

SoundDeviceContextPair &UIApp::GetSoundDeviceContextPair()
{
    if(!UIApp::GetGlobalApp() || !UIApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::UIAppNotInitialized, "UIApp is not initialized");
    return *m_soundDeviceContextPair.get();
}

FontLibrary &UIApp::GetFontLibrary()
{
    if(!UIApp::GetGlobalApp() || !UIApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::UIAppNotInitialized, "UIApp is not initialized");
    return *m_fontLibrary.get();
}

bool UIApp::IsInitialized() const
{
    return m_initialized;
}

UIApp::~UIApp()
{
}


ARX_NAMESPACE_END
