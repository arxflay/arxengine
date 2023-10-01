#include "ArxWindow.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
ARX_NAMESPACE_BEGIN


ArxWindow::ArxWindow(std::string_view title, Size size, Position position, int attributes)
    : UIObject(nullptr, ArxWindow::GetRectSize() < size ? size : ArxWindow::GetRectSize(), position)
    , m_title(title)
{
    GLFWwindow *win = glfwCreateWindow(ArxWindow::GetSize().width, ArxWindow::GetSize().height, title.data(), nullptr, nullptr);
    SetGLFWwindow(win);
    SetWindowAttributes(attributes);
    glfwSetWindowUserPointer(GetGLFWwindow(), this);
    glfwSetWindowPos(GetGLFWwindow(), position.x, position.y);
    InitGlfwCallbacks();
    //if (parent == nullptr)
    //TODO currently non TopLevelWindows aren't supported
    ArxTheApp->GetTopLevelWindowManager().AddTopLevelWindow(this);

    GetEventHandler().Bind<CloseEvent>(std::function<void(CloseEvent&)>([](CloseEvent&e) {
        ArxWindow *arxWin = dynamic_cast<ArxWindow*>(e.GetCallingObject());
        /*if (arxWin->GetParent())
            arxWin->SetParent(nullptr);
        else*/
        
        ArxTheApp->GetTopLevelWindowManager().RemoveTopLevelWindow(arxWin);
        
        e.Skip();
    }));
}

void ArxWindow::InitGlfwCallbacks()
{
    glfwSetWindowCloseCallback(GetGLFWwindow(), &ArxWindow::close_callback); 
    glfwSetWindowFocusCallback(GetGLFWwindow(), &ArxWindow::focus_callback);
    glfwSetWindowSizeCallback(GetGLFWwindow(),  &ArxWindow::size_callback);
    glfwSetWindowPosCallback(GetGLFWwindow(), &ArxWindow::position_callback);
}

/*static*/ void ArxWindow::close_callback(GLFWwindow *win)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->Destroy();
}

/*static*/ void ArxWindow::focus_callback(GLFWwindow *win, int focused)
{
    if (focused)
    {
        glfwMakeContextCurrent(win);
        ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
        glViewport(0, 0, arxWin->GetSize().width, arxWin->GetSize().height);
    }
}

/*static*/ void ArxWindow::size_callback(GLFWwindow *win, int width, int height)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->UIObject::SetSize(Size{ width, height });
    glViewport(0, 0, arxWin->GetSize().width, arxWin->GetSize().height);
}

/*static*/ void ArxWindow::position_callback(GLFWwindow *win, int x, int y)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->UIObject::SetPosition(Position{ x, y });
}

void ArxWindow::SetPosition(const Position &pos)
{
    glfwSetWindowPos(GetGLFWwindow(), pos.x, pos.y);
}

Size ArxWindow::GetRectSize() 
{
    return Size{ 1, 1 };
}

void ArxWindow::Draw()
{
    glm::vec3 normalizedColor = GetBackgroundColor().GetNormalizedRGB(); 
    glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (UIObject *obj : GetChildren())
        if (dynamic_cast<ArxWindow*>(obj) == nullptr)
            obj->Draw();

    glfwSwapBuffers(GetGLFWwindow());
}

void ArxWindow::SetMinSize(const Size &s)
{
    UIObject::SetMinSize(s);
    glfwSetWindowSizeLimits(GetGLFWwindow(), GetMinSize().width, GetMinSize().height, GetMaxSize().width, GetMaxSize().height);
}

void ArxWindow::SetMaxSize(const Size &s)
{
    UIObject::SetMaxSize(s);
    glfwSetWindowSizeLimits(GetGLFWwindow(), GetMinSize().width, GetMinSize().height, GetMaxSize().width, GetMaxSize().height);
}

void ArxWindow::SetSize(const Size &s)
{
    glfwSetWindowSize(GetGLFWwindow(), s.width, s.height);
}

void ArxWindow::Show(bool visible)
{
    (visible) ? glfwShowWindow(GetGLFWwindow()) : glfwHideWindow(GetGLFWwindow());
}

void ArxWindow::SetFocus(bool focus)
{
    if (focus)
    {
        glfwRequestWindowAttention(GetGLFWwindow());
        glfwMakeContextCurrent(GetGLFWwindow());
    }
}

void ArxWindow::SetWindowAttributes(int attributes)
{
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_RESIZABLE, !!(attributes & WindowAttributes::RESIZABLE));
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_DECORATED, !!(attributes & WindowAttributes::DECORATED));
    glfwSetWindowAttrib(GetGLFWwindow(), GLFW_FLOATING,  !!(attributes & WindowAttributes::DISPLAY_ON_TOP));
}

std::string_view ArxWindow::GetTitle()
{
    return m_title;
}

void ArxWindow::SetTitle(std::string_view title)
{
    m_title = title;
    glfwSetWindowTitle(GetGLFWwindow(), GetTitle().data());
}

GLFWwindow *ArxWindow::GetGLFWwindow()
{
    return m_win.get();
}

void ArxWindow::SetGLFWwindow(::GLFWwindow *window)
{
    m_win = decltype(m_win)(window, &glfwDestroyWindow);
}

ARX_NAMESPACE_END
