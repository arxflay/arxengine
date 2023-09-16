#include "UIApp.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <functional>
#include "Evt.h"

ARX_NAMESPACE_BEGIN

std::unique_ptr<UIApp> g_app;

/*static*/ void UIApp::SetInstance(UIApp *app)
{
    (void)g_app.release();
    g_app = std::unique_ptr<UIApp>(app);
}

/*static*/ UIApp *UIApp::GetInstance()
{
    return g_app.get();
}

UIApp::UIApp()
    : m_initialized(false)
{
}

bool UIApp::InitGlfw()
{
    if (!glfwInit())
        return false;
    
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, defaults::GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, defaults::GL_VERSION_MINOR);

    return true;
}

int UIApp::Init()
{
    if (!m_initialized)
    {
        if (!InitGlfw())
            return -1;
        if (!InitGlad())
            return -2;

        m_initialized = true;
    }

    return 0;
}

EventLoop &UIApp::GetEventLoop()
{
    return m_evtLoop;
}

void UIApp::OnExec()
{
}

int UIApp::Exec()
{
    OnExec();
    return m_evtLoop.Run();
}

bool UIApp::InitGlad()
{
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> tempWin(glfwCreateWindow(1, 1, "", nullptr, nullptr), [](GLFWwindow *win) {
        glfwMakeContextCurrent(nullptr);
        glfwDestroyWindow(win);        
    });
    glfwMakeContextCurrent(tempWin.get());
    bool ret = !!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(&glfwGetProcAddress));
    
    return ret;
}

UIApp::~UIApp()
{
    glfwTerminate();
}



ARX_NAMESPACE_END
