#include "ArxWindow.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>

ARX_NAMESPACE_BEGIN

namespace
{
    Size MakeSizeValid(Size s)
    {
        if (s.height < 1 || s.width < 1)
            return defaults::DEFAULT_SIZE;

        return Size::Truncate(s);
    }
    
    void CloseCallback(GLFWwindow *win)
    {
        ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
        arxWin->Destroy();
    }
    
    void FocusCallback(GLFWwindow *win, int focused)
    {
        if (focused)
        {
            ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
            arxWin->SetAsCurrentContext();
            glViewport(0, 0, static_cast<GLsizei>(arxWin->GetSize().width), static_cast<GLsizei>(arxWin->GetSize().height));
            arxWin->Draw();
        }
    }

    void SizeCallback(GLFWwindow *win, int width, int height)
    {
        ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
        arxWin->UIObject::SetSize(Size(width, height));
    }

    void PositionCallback(GLFWwindow *win, int x, int y)
    {
        ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
        arxWin->UIObject::SetPosition(Position(x, y));
    }

    void SetGlfwCallbacks(GLFWwindow *win)
    {
        glfwSetWindowCloseCallback(win, CloseCallback); 
        glfwSetWindowFocusCallback(win, FocusCallback);
        glfwSetWindowSizeCallback(win,  SizeCallback);
        glfwSetWindowPosCallback(win, PositionCallback);
    }
}

//uncompelte fullscreen
ArxWindow::ArxWindow(std::string_view title, Size size , Position position, int attributes) //, bool isFullScreen)
    : UIObject(nullptr, size, position), m_win(nullptr, glfwDestroyWindow), m_attributes(attributes), m_title(title)
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
        UIObject::SetSize(MakeSizeValid(size));
        m_win.reset(glfwCreateWindow(static_cast<int>(GetSize().width), static_cast<int>(GetSize().height), m_title.c_str(), nullptr, nullptr));
    //}
    if (!m_win)
        throw ArxException(ArxException::ErrorCode::FailedToConstructArxWindow, "failed to create opengl context");

    glfwSetWindowPos(m_win.get(), static_cast<int>(position.x) , static_cast<int>(position.y + GetSize().height));
    glfwSetWindowUserPointer(m_win.get(), this);
    SetWindowAttributes(m_attributes);
    SetGlfwCallbacks(m_win.get());
    GetEventManager().Bind<DrawEvent>(std::function<void(DrawEvent&)>([](DrawEvent &){ 

        std::cout << 10 << '\n';
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT); }));
    //SetAsCurrentContext();
    //m_viewport = glm::ortho(0.0f, size.width, 0.0f, size.height);
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
    Size newSize = MakeSizeValid(s);
    glfwSetWindowSize(m_win.get(), static_cast<int>(newSize.height), static_cast<int>(newSize.width));
}

void ArxWindow::SetPosition(Position pos)
{
    glfwSetWindowPos(m_win.get(), static_cast<int>(pos.x), static_cast<int>(pos.y + GetSize().width));
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

ARX_NAMESPACE_END
