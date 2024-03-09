#include "GameApp.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "internal/UniversalExceptionHandler.h"
#include "logging/Logger.h"
#include "evt/UIEventProcessor.h"
#include "ArxObject.h"
#include "ui/ArxWindow.h"
#include "internal/al/SoundDevice.h"
#include "internal/ft/FontLoader.h"

ARX_NAMESPACE_BEGIN

namespace internal
{
    std::pair<std::unique_ptr<SoundDevice>, std::unique_ptr<SoundContext>> InitAL()
    {
        GLOG->Info("Initializing OpenAL...");
        GLOG->Info("Creating OpenAL default device");
        std::unique_ptr<SoundDevice> soundDevice(std::make_unique<SoundDevice>());
        if (soundDevice->IsInvalid())
            throw ArxException(ArxException::ErrorCode::FailedToInitializeOpenAL, "Failed to create default OpenAL device");
        GLOG->Info("Creating OpenAL default device context");
        std::unique_ptr<SoundContext> soundContext(std::make_unique<SoundContext>(soundDevice->CreateContext()));
        if (soundContext->IsInvalid())
            throw ArxException(ArxException::ErrorCode::FailedToInitializeOpenAL, "Failed to create default OpenAL device context");

        return std::make_pair(std::move(soundDevice), std::move(soundContext));
    }

    std::unique_ptr<FontLoader> InitFT()
    {
        GLOG->Info("Initializing Freetype...");
        std::unique_ptr<FontLoader> lib(std::make_unique<FontLoader>());
        if (lib->IsInvalid())
            throw ArxException(ArxException::ErrorCode::FailedToInitializeFreetype, "Failed to initialize freetype");

        return lib;
    }

    void InitGLFW()
    {
        GLOG->Info("Initializing GLFW...");
        int err = glfwInit();
        if (err != GLFW_TRUE)
            throw ArxException(ArxException::ErrorCode::FailedToInitializeFreetype, Utils::Format("Failed to initialize glfw library, glfw_error_code='%d'", err));
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
        auto m_soundDeviceContextPair = internal::InitAL();
        m_soundDevice = std::move(m_soundDeviceContextPair.first);
        m_soundContext = std::move(m_soundDeviceContextPair.second);
        m_fontLoader = internal::InitFT();
        m_eventProcessor = std::make_unique<UIEventProcessor>();
        OnAfterInit();
        m_initialized = true;
        GLOG->Info("Initialization done");
    }
    catch(...)
    {
        code = static_cast<int>(UniversalExceptionHandler::HandleException());
        GLOG->Info("Initialization failed");
        m_soundContext.reset();
        m_soundDevice.reset();
        m_fontLoader.reset();
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
        for (ArxWindow *win : m_windows)
            win->Draw();
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

SoundContext &GameApp::GetDefaultSoundDeviceContext()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    return *m_soundContext.get();
}

FontLoader &GameApp::GetFontLoader()
{
    if(!GameApp::GetGlobalApp() || !GameApp::GetGlobalApp()->IsInitialized())
        throw ArxException(ArxException::ErrorCode::GameAppNotInitialized, "GameApp is not initialized");
    return *m_fontLoader.get();
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
