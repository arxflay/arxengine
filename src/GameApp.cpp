#include "GameApp.h"
#include "internal/UniversalExceptionHandler.h"
#include "SoundPlayer.h"
#include "Font.h"
#include "logging/Logger.h"
#include "UIEventProcessor.h"
#include "ArxObject.h"
#include "ArxWindow.h"

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

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, defaults::GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, defaults::GL_VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }
}

void DeleteEvent::HandleEvent()
{
    if (GetSender())
        GameApp::GetGlobalApp()->m_deleteQueue.emplace(GetSender());
}

std::unique_ptr<GameApp> g_app;

/*static*/ int GameApp::SetAppAsGlobal(std::unique_ptr<GameApp> &&app)
{
    if (GetGlobalApp() != nullptr && GetGlobalApp()->IsInitialized())
        return static_cast<int>(ArxException::ErrorCode::GameAppAlreadyInitialized);

    g_app = std::move(app);
    return 0;
}

/*static*/ GameApp *GameApp::GetGlobalApp()
{
    return g_app.get();
}

GameApp::GameApp()
    : m_initialized(false)
    , m_eventProcessor(nullptr)
    , m_soundDeviceContextPair(nullptr, DestroySoundDeviceContextPair)
    , m_fontLibrary(nullptr, DestroyFontLibrary)
{
    CleanUp();
}

int GameApp::Init()
{
    int code = static_cast<int>(ArxException::ErrorCode::NoError);
    try
    {
        GLOG->Info("Initialization begin");
        if (GetGlobalApp() != this)
            throw ArxException(ArxException::ErrorCode::GameAppNotSetAsGlobal, "This app is not set as global");
        else if (m_initialized)
            throw ArxException(ArxException::ErrorCode::GameAppAlreadyInitialized, "App is already inititalized");
    
        internal::InitGLFW();
        internal::InitGlad();
        m_soundDeviceContextPair.reset(internal::InitAL());
        m_fontLibrary.reset(internal::InitFT());
        m_eventProcessor = std::make_unique<UIEventProcessor>();
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

int GameApp::Run()
{
    if (m_running)
        return static_cast<int>(ArxException::ErrorCode::GameAppIsRunning); //TODO
    
    m_running = true;
    OnRun();
    
    double m_past = glfwGetTime();
    double m_now = 0; 
    
    while(!m_shouldExit)
    {
        m_now = glfwGetTime();
        m_deltaTime = m_now - m_past;
        m_past = m_now;
        glfwPollEvents();
        m_eventProcessor->ProcessEvents();
        try
        {
            while(!m_deleteQueue.empty())
            {
                ArxObject *obj = m_deleteQueue.front();
                m_deleteQueue.pop();
                delete obj;
            }
        }
        catch(...) //TODO
        {
            /*do nothing*/
            UniversalExceptionHandler::HandleException();
        }
    }

    //cleanup
    int oldExitCode = m_exitCode;
    CleanUp(); 

    return oldExitCode;
}

void GameApp::CleanUp()
{
    if (m_eventProcessor)
        m_eventProcessor->RemoveEvents();
    while(!m_windows.empty())
    {
        ArxWindow *win = *m_windows.begin();
        m_windows.erase(m_windows.begin());
        delete win;
    }
    m_shouldExit = false;
    m_exitCode = 0;
    m_running = false;
    m_deltaTime = 0;
}

const ArxWindowSet &GameApp::GetWindowSet()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    return m_windows;
}

int GameApp::Exit(int code)
{
    if (m_shouldExit)
        return static_cast<int>(ArxException::ErrorCode::GameAppAlreadyScheduledExit);
    m_shouldExit = true;
    m_exitCode = code;

    return static_cast<int>(ArxException::ErrorCode::NoError);
}

SoundDeviceContextPair &GameApp::GetSoundDeviceContextPair()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    return *m_soundDeviceContextPair.get();
}

FontLibrary &GameApp::GetFontLibrary()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    return *m_fontLibrary.get();
}

bool GameApp::IsInitialized() const
{
    return m_initialized;
}

GameApp::~GameApp()
{
    if (m_initialized)
    {
        glfwTerminate();
        CleanUp();
    }
}

double GameApp::GetDeltaTime() const
{
    return m_deltaTime;
}

EventProcessor &GameApp::GetEventProcessor()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    
    return *m_eventProcessor.get();
}


ARX_NAMESPACE_END
