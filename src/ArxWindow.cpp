#include "ArxWindow.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <iostream>
ARX_NAMESPACE_BEGIN

namespace
{
    Size MakeSizeValid(Size s)
    {
        if (s.height < 1 || s.width < 1)
            return defaults::DEFAULT_SIZE;

        return Size::Truncate(s);
    }
}

/*static*/ void ArxWindow::PositionCallback(GLFWwindow *win, int x, int y)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    int xOffset, yOffset;
    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &xOffset, &yOffset);
    int leftBorder, topBorder = 0;
    glfwGetWindowFrameSize(arxWin->m_win.get(), &leftBorder, &topBorder, nullptr, nullptr);
    arxWin->UIObject::SetPosition(Position(static_cast<float>(x - xOffset - leftBorder), static_cast<float>(y - yOffset - topBorder)));
}


/*static*/ void ArxWindow::CloseCallback(GLFWwindow *win)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->RequestDelete();
}

/*static*/ void ArxWindow::RefreshCallback(GLFWwindow *win)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    arxWin->SetAsCurrentContext();
    int width = 0, height = 0;
    glfwGetWindowSize(arxWin->m_win.get(), &width, &height);
    arxWin->RecalculateSizes(Size(width, height));
    glViewport(0, 0, static_cast<GLsizei>(arxWin->GetClientSize().width), static_cast<GLsizei>(arxWin->GetClientSize().height));
    arxWin->Draw();
}

/*static*/ void ArxWindow::SetGlfwCallbacks(GLFWwindow *win)
{
    glfwSetWindowCloseCallback(win, ArxWindow::CloseCallback); 
    glfwSetWindowPosCallback(win, ArxWindow::PositionCallback);
    glfwSetWindowRefreshCallback(win, ArxWindow::RefreshCallback);
}

//uncompelte fullscreen
ArxWindow::ArxWindow(std::string_view title, Size size , Position position, int attributes) //, bool isFullScreen)
    : m_win(nullptr, glfwDestroyWindow), m_attributes(attributes), m_title(title)
{
    /*if (isFullScreen) //TODO fullscreen
    {
        
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *currentVideoMode = glfwGetVideoMode(primaryMonitor);
        UIObject::SetPosition(Position{ 0, 0 });
        UIObject::SetSize(Size{ static_cast<float>(currentVideoMode->width), static_cast<float>(currentVideoMode->height) });
        m_win.reset(glfwCreateWindow(static_cast<int>(GetSize().width), static_cast<int>(GetSize().height), m_title.c_str(), primaryMonitor, nullptr));
    }
    else
    {*/
        Size validSize = MakeSizeValid(size); 
        m_win.reset(glfwCreateWindow(static_cast<int>(validSize.height), static_cast<int>(validSize.width), m_title.c_str(), nullptr, nullptr));
    //}
    if (!m_win)
        throw ArxException(ArxException::ErrorCode::FailedToConstructArxWindow, "failed to create opengl context");
    glfwSetWindowUserPointer(m_win.get(), this);

    SetWindowAttributes(m_attributes);
    RecalculateSizes(validSize);
    SetPosition(position);
    SetGlfwCallbacks(m_win.get());
    GetEventManager().Bind<DrawEvent>(std::function<void(DrawEvent&)>([this](DrawEvent &){ 
        glm::vec4 normalizedColor = GetColor().GetNormalizedColorRGBA();
        glClearColor(normalizedColor.r, normalizedColor.g, normalizedColor.b, normalizedColor.a); 
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(m_win.get());
    }));

    //TODO SET CLIENT_SIZE
    //m_viewport = glm::ortho(0.0f, size.width, 0.0f, size.height);
    RegisterWindowFromWindowList();
}

class ShowWindowEvent : public Event
{
public:
    ShowWindowEvent(GLFWwindow *window, bool show = true)
        : m_window(window)
        , m_show(show)
    {
    }
    void HandleEvent()
    {
        if (m_show)
            glfwShowWindow(m_window);
        else
            glfwHideWindow(m_window);
    }
private:
    GLFWwindow *m_window;
    bool m_show;
};

void ArxWindow::Show(bool visible) 
{
    GetEventManager().QueueEvent<ShowWindowEvent>(std::make_unique<ShowWindowEvent>(m_win.get(), visible));
}

void ArxWindow::SetWindowAttributes(int attributes)
{
    glfwSetWindowAttrib(m_win.get(), GLFW_RESIZABLE, !!(attributes & WindowAttributes::RESIZABLE));
    glfwSetWindowAttrib(m_win.get(), GLFW_DECORATED, !!(attributes & WindowAttributes::DECORATED));
    glfwSetWindowAttrib(m_win.get(), GLFW_FLOATING,  !!(attributes & WindowAttributes::DISPLAY_ON_TOP));
    m_attributes = attributes;
}

int ArxWindow::GetAttributes() const
{
    return m_attributes;
}

std::string_view ArxWindow::GetTitle()
{
    return m_title;
}

void ArxWindow::SetTitle(std::string_view title)
{
    glfwSetWindowTitle(m_win.get(), title.data());
}

void ArxWindow::SetSize(Size s)
{
    RecalculateSizes(s);
    glfwSetWindowSize(m_win.get(), static_cast<int>(GetSize().height), static_cast<int>(GetSize().width));
}

void ArxWindow::RecalculateSizes(Size s)
{
    Size newClientSize = MakeSizeValid(s);
    m_clientSize = newClientSize;
    WindowBorders borders = GetWindowBorders();
    Size newSize = Size(newClientSize.width + static_cast<float>(borders.left + borders.right), newClientSize.height + static_cast<float>(borders.bottom + borders.top));
    UIObject::SetSize(newSize);
}

void ArxWindow::RegisterWindowFromWindowList()
{
    const_cast<ArxWindowSet&>(GameApp::GetGlobalApp()->GetWindowSet()).insert(this);
}

void ArxWindow::UnregisterWindowFromWindowList()
{
    const_cast<ArxWindowSet&>(GameApp::GetGlobalApp()->GetWindowSet()).erase(this);
}

Size ArxWindow::GetClientSize() const
{
    return m_clientSize;
}

ArxWindow::WindowBorders ArxWindow::GetWindowBorders() const
{
    ArxWindow::WindowBorders borders;
    glfwGetWindowFrameSize(m_win.get(), &borders.left, &borders.top, &borders.right, &borders.bottom);
    return borders;
}
void ArxWindow::SetPosition(Position pos)
{
    UIObject::SetPosition(pos);
    int xOffset, yOffset;
    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &xOffset, &yOffset);
    glfwSetWindowPos(m_win.get(), static_cast<int>(pos.x) + xOffset, static_cast<int>(pos.y) + yOffset);
}

Position ArxWindow::GetRealPosition()
{
    int x, y;
    glfwGetWindowPos(m_win.get(), &x, &y);
    return Position(x, y);
}

void ArxWindow::SetAsCurrentContext()
{
    glfwMakeContextCurrent(m_win.get());
}

/*
void ArxWindow::SetFixedViewport(float width, float height)
{
    m_useFixedViewport = true;
    m_viewport = glm::ortho(0.0f, width, 0.0f, height);
    Draw();
}

void ArxWindow::RemoveFixedViewport()
{
    m_useFixedViewport = false;
    m_viewport = glm::ortho(0.0f, GetSize().width, 0.0f, GetSize().height);
    Draw();
}

const glm::mat4 &ArxWindow::GetViewport()
{
    return m_viewport;
}
*/

ArxWindow::~ArxWindow()
{
    UnregisterWindowFromWindowList();
    if (GameApp::GetGlobalApp()->GetWindowSet().empty())
        GameApp::GetGlobalApp()->Exit(0);
}

void ArxWindow::Reparent(ArxObject*)
{
    //for now ArxWindow can't have parents
}

ARX_NAMESPACE_END
