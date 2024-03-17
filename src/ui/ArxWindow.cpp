#include "ui/ArxWindow.h"
#include "ui/internal/UICache.h"
#include "ui/Painter.h"
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include "media/Image.h"
#include <memory>
#include <iostream>
#include "ui/KeyEvent.h"
#include "ui/MouseEvent.h"

ARX_NAMESPACE_BEGIN

namespace
{
    Size MakeSizeValid(Size s)
    {
        if (s.height < 1 || s.width < 1)
            return constants::DEFAULT_SIZE;

        return Size::Truncate(s);
    }
}

/*static*/ void ArxWindow::PositionCallback(GLFWwindow *win, int x, int y)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    int xOffset, yOffset;
    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &xOffset, &yOffset);
    ArxWindow::WindowBorders borders = arxWin->GetWindowBorders(); 
    arxWin->UIControl::SetPosition(Position(static_cast<float>(x - xOffset - borders.left), static_cast<float>(y - yOffset - borders.top)));
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
}


/*static*/ void ArxWindow::KeyboardInputCallback(GLFWwindow* window, int key, int, int action, int)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(window));
    std::unique_ptr<KeyEvent> keyEvent;
    switch(action)
    {
        case GLFW_PRESS:
            keyEvent = std::make_unique<KeyDownEvent>();
            keyEvent->SetKey(static_cast<KeyEvent::Key>(key));
            arxWin->GetEventManager().QueueEvent<KeyDownEvent>(std::move(keyEvent));
            break;
        case GLFW_RELEASE:
            keyEvent = std::make_unique<KeyUpEvent>();
            keyEvent->SetKey(static_cast<KeyEvent::Key>(key));
            arxWin->GetEventManager().QueueEvent<KeyUpEvent>(std::move(keyEvent));
            break;
        case GLFW_REPEAT:
            keyEvent = std::make_unique<KeyHoldEvent>();
            keyEvent->SetKey(static_cast<KeyEvent::Key>(key));
            arxWin->GetEventManager().QueueEvent<KeyHoldEvent>(std::move(keyEvent));
            break;
        default:
            break;
    }
}

class ShowCursorEvent : public Event 
{
public:
    ShowCursorEvent(GLFWwindow *win, bool &showOutputResult, bool shouldShow)
        : m_win(win), m_shouldShowCursor(shouldShow), m_showCursor(showOutputResult)
    {
    }
private:
    void HandleEvent() 
    {
        ArxWindow *win = static_cast<ArxWindow*>(GetSender());
        m_showCursor = m_shouldShowCursor;
        glfwSetInputMode(m_win, GLFW_CURSOR, m_showCursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        if (!m_showCursor && !win->m_mouseEnteredControls.empty())
            win->SendForcefullyMouseExitEvents();
    }
    GLFWwindow *m_win;
    bool m_shouldShowCursor;
    bool &m_showCursor;
};

void ArxWindow::ShowCursor(bool show)
{
    GetEventManager().QueueEvent<ShowCursorEvent>(std::make_unique<ShowCursorEvent>(m_win.get(), m_showCursor, show));
}

Position ArxWindow::GetCursorPosition()
{
    double x, y;
    glfwGetCursorPos(m_win.get(), &x, &y);
    return Position(x, y);
}

namespace
{
    UIControl *FindCandidateForMouseInput(UIControl *candidate, Position pos)
    {
        for (ArxObject *obj : candidate->GetChildren())
        {
            UIControl *control = dynamic_cast<UIControl*>(obj); 
            if (control && control->IsShown() && control->HitTest(pos))
                return FindCandidateForMouseInput(control, pos);
        }

        return candidate;
    }
        
    UIControl *FindCandidateForMouseInput(ArxWindow *win)
    {
        if (!win->IsCursorVisible())
            return win;

        Position viewportAffectedPosition = GetViewportAffectedPosition(win->GetViewport(), win->GetClientSize(), win->GetCursorPosition());
        return FindCandidateForMouseInput(win, viewportAffectedPosition);    
    }
}

bool ArxWindow::IsCursorVisible() const
{
    return m_showCursor;
}

/*static*/ void ArxWindow::MouseInputCallback(GLFWwindow *win, int button, int action, int)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    std::unique_ptr<MouseButtonEvent> mouseEvent;
    switch(action)
    {
        case GLFW_PRESS:
            {
                UIControl *candidate = FindCandidateForMouseInput(arxWin);
                mouseEvent = std::make_unique<MouseDownEvent>();
                mouseEvent->SetMouseButtonType(static_cast<MouseButtonEvent::ButtonType>(button));
                candidate->GetEventManager().QueueEvent<MouseDownEvent>(std::move(mouseEvent));
                arxWin->m_pressedMouseButtons.emplace(candidate, static_cast<MouseButtonEvent::ButtonType>(button));
            }
            break;
        case GLFW_RELEASE:
            {
                auto it = arxWin->m_pressedMouseButtons.find({nullptr, static_cast<MouseButtonEvent::ButtonType>(button)});
                if (it != arxWin->m_pressedMouseButtons.end())
                {
                    arxWin->m_pressedMouseButtons.erase(it);
                    mouseEvent = std::make_unique<MouseUpEvent>();
                    UIControl *control = it->control;
                    mouseEvent->SetMouseButtonType(static_cast<MouseButtonEvent::ButtonType>(button));
                    control->GetEventManager().QueueEvent<MouseUpEvent>(std::move(mouseEvent));
                }
            }
            break;
        default:
            break;
    }
}

void ArxWindow::SendMouseEnterExitEvents(UIControl *ctrl, Position pos)
{
    for (ArxObject *obj : ctrl->GetChildren())
    {
        UIControl *control = dynamic_cast<UIControl*>(obj);
        if (control)
        {
            bool alreadyEntered = m_mouseEnteredControls.count(control);
            bool hitted = control->HitTest(pos);
            if (hitted && !alreadyEntered && control->IsShown())
            {
                m_mouseEnteredControls.emplace(control);
                if (control->GetEventManager().HasNonDefaultEventHandler<MouseEnterEvent>())
                    control->GetEventManager().QueueEvent<MouseEnterEvent>(std::make_unique<MouseEnterEvent>());
            }
            else if (!hitted && alreadyEntered)
            {
                m_mouseEnteredControls.erase(control);
                if (control->GetEventManager().HasNonDefaultEventHandler<MouseExitEvent>())
                    control->GetEventManager().QueueEvent<MouseExitEvent>(std::make_unique<MouseExitEvent>());
            }
            SendMouseEnterExitEvents(control, pos);
        }
    }
}

void ArxWindow::SendMouseEnterExitEvents(Position pos)
{
    (void)SendMouseEnterExitEvents(this, pos);
}

void ArxWindow::SendForcefullyMouseExitEvents()
{
    for (UIControl *control : m_mouseEnteredControls)
    {
        if (control->GetEventManager().HasNonDefaultEventHandler<MouseExitEvent>())
        {
            std::unique_ptr<MouseExitEvent> evt = std::make_unique<MouseExitEvent>();
            evt->SetWasForced(true);
            control->GetEventManager().QueueEvent<MouseExitEvent>(std::make_unique<MouseExitEvent>());
        }
    }
    m_mouseEnteredControls.clear();
}

void ArxWindow::CursorPosCallback(GLFWwindow *win, double xpos, double ypos)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    Position pos(xpos, ypos);
    Position viewportAffectedPosition(GetViewportAffectedPosition(arxWin->GetViewport(), arxWin->GetClientSize(), pos));
    
    if (arxWin->GetEventManager().HasNonDefaultEventHandler<MouseMoveEvent>())
    {
        std::unique_ptr<MouseMoveEvent> evt = std::make_unique<MouseMoveEvent>();
        evt->SetPosition(pos);
        evt->SetViewportAffectedPosition(viewportAffectedPosition);
        arxWin->GetEventManager().QueueEvent<MouseMoveEvent>(std::move(evt)); 
    }

    if (arxWin->IsCursorVisible())
        arxWin->SendMouseEnterExitEvents(viewportAffectedPosition);
}

/*static*/ void ArxWindow::MouseEnterExitCallback(GLFWwindow *win, int enter)
{
    ArxWindow *arxWin = static_cast<ArxWindow*>(glfwGetWindowUserPointer(win));
    if (enter && arxWin->GetEventManager().HasNonDefaultEventHandler<MouseEnterEvent>())
        arxWin->GetEventManager().QueueEvent<MouseEnterEvent>(std::make_unique<MouseEnterEvent>());
    else if (arxWin->GetEventManager().HasNonDefaultEventHandler<MouseExitEvent>())
        arxWin->GetEventManager().QueueEvent<MouseExitEvent>(std::make_unique<MouseExitEvent>());
}

/*static*/ void ArxWindow::SetGlfwCallbacks(GLFWwindow *win)
{
    glfwSetWindowCloseCallback(win, ArxWindow::CloseCallback); 
    glfwSetWindowPosCallback(win, ArxWindow::PositionCallback);
    glfwSetWindowRefreshCallback(win, ArxWindow::RefreshCallback);
    glfwSetKeyCallback(win, ArxWindow::KeyboardInputCallback);
    glfwSetMouseButtonCallback(win, ArxWindow::MouseInputCallback);
    glfwSetCursorPosCallback(win, ArxWindow::CursorPosCallback);
    glfwSetCursorEnterCallback(win, ArxWindow::MouseEnterExitCallback);
}

//uncompelte fullscreen
ArxWindow::ArxWindow(std::string_view title, Size size, Position position, int attributes) //, bool isFullScreen)
    : m_win(nullptr, glfwDestroyWindow)
    , m_attributes(attributes)
    , m_title(title)
    , m_useFixedViewport(false)
    , m_uiCache(std::make_unique<UICache>())
    , m_vsyncEnabled(false)
    , m_showCursor(true)
{
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, constants::GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, constants::GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /*if (isFullScreen) //TODO fullscreen
    {
        
        GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *currentVideoMode = glfwGetVideoMode(primaryMonitor);
        UIControl::SetPosition(Position{ 0, 0 });
        UIControl::SetSize(Size{ static_cast<float>(currentVideoMode->width), static_cast<float>(currentVideoMode->height) });
        m_win.reset(glfwCreateWindow(static_cast<int>(GetSize().width), static_cast<int>(GetSize().height), m_title.c_str(), primaryMonitor, nullptr));
    }
    else
    {*/
        Size validSize = MakeSizeValid(size); 
        m_win.reset(glfwCreateWindow(static_cast<int>(validSize.width), static_cast<int>(validSize.height), m_title.c_str(), nullptr, nullptr));
    //}
    if (!m_win)
    {
        GLOG->Error("Failed to create window. Probably platform doesn't support OpenGL %d %d", constants::GL_VERSION_MAJOR, constants::GL_VERSION_MINOR);
        throw ArxException(ArxException::ErrorCode::FailedToConstructArxWindow, "failed to create opengl context");
    }

    SetAsCurrentContext();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw ArxException(ArxException::ErrorCode::FailedToConstructArxWindow, "failed to initialize glad");

    glfwSetWindowUserPointer(m_win.get(), this);

    SetWindowAttributes(m_attributes);
    RecalculateSizes(validSize);
    SetPosition(position);
    SetGlfwCallbacks(m_win.get());

    GetEventManager().Bind<ShowEvent>(&ArxWindow::OnShow, this);
    m_viewport = Viewport{glm::ortho(0.0f, size.width, 0.0f, size.height), size };
    m_uiCache->Init();
    RegisterWindowFromWindowList();
    glfwSwapInterval(1);
}

void ArxWindow::OnDraw(DrawEvent &e)
{
    void(this);
    Painter painter(e);
    painter.Clear();
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
    if (!m_useFixedViewport)
        m_viewport = Viewport{ glm::ortho(0.0f, m_clientSize.width, 0.0f, m_clientSize.height), m_clientSize };

    WindowBorders borders = GetWindowBorders();
    Size newSize = Size(newClientSize.width + static_cast<float>(borders.left + borders.right), newClientSize.height + static_cast<float>(borders.bottom + borders.top));
    UIControl::SetSize(newSize);
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
    UIControl::SetPosition(pos);
    int xOffset, yOffset;
    glfwGetMonitorPos(glfwGetPrimaryMonitor(), &xOffset, &yOffset);
    WindowBorders borders = GetWindowBorders();
    glfwSetWindowPos(m_win.get(), static_cast<int>(pos.x) + xOffset + borders.left, static_cast<int>(pos.y) + yOffset + borders.top);
}

Position ArxWindow::GetRealPosition() const
{
    int x, y;
    glfwGetWindowPos(m_win.get(), &x, &y);
    return Position(x, y);
}

ArxWindow *ArxWindow::GetWindow() 
{
    return this;
}

const ArxWindow *ArxWindow::GetWindow() const
{
    return this;
}

void ArxWindow::SetAsCurrentContext() const
{
    glfwMakeContextCurrent(m_win.get());
}

void ArxWindow::SetFixedViewport(float width, float height)
{
    m_useFixedViewport = true;
    m_viewport = Viewport{glm::ortho(0.0f, width, 0.0f, height), Size(width, height)};
}

void ArxWindow::RemoveFixedViewport()
{
    m_useFixedViewport = false;
    m_viewport = Viewport{glm::ortho(0.0f, GetClientSize().width, 0.0f, GetClientSize().height), GetClientSize() };
}

const Viewport &ArxWindow::GetViewport() const
{
    return m_viewport;
}

ArxWindow::~ArxWindow()
{
    SetAsCurrentContext();
    UnregisterWindowFromWindowList();
    if (GameApp::GetGlobalApp()->GetWindowSet().empty())
        GameApp::GetGlobalApp()->Exit(0);
}

void ArxWindow::Reparent(ArxObject*)
{
    //for now ArxWindow can't have parents
}

UICache *ArxWindow::GetUICache()
{
    return m_uiCache.get();
}

/*static*/ void ArxWindow::DrawInternal(UIControl *obj)
{
    if (!obj->IsShown())
        return;

    std::unique_ptr<DrawEvent> evt(new DrawEvent);
    obj->GetEventManager().QueueEvent<DrawEvent>(std::move(evt));
    for (ArxObject *obj : const_cast<ArxObjectList&>(obj->GetChildren()))
    {
        UIControl *uiControl = dynamic_cast<UIControl*>(obj);
        if (uiControl)
            DrawInternal(uiControl);
    }
}

class SwapBuffersEvent : public Event
{
public:
    SwapBuffersEvent(GLFWwindow *win)
        : m_win(win)
    {
    }
private:
    void HandleEvent()
    {
        glfwSwapBuffers(m_win);
    }
    GLFWwindow *m_win;
};

void ArxWindow::Draw()
{
    if (!IsShown())
        return;

    DrawInternal(this);
    std::unique_ptr<SwapBuffersEvent> evt(std::make_unique<SwapBuffersEvent>(m_win.get()));
    GetEventManager().QueueEvent<SwapBuffersEvent>(std::move(evt));
}

void ArxWindow::DrawNowInternal(UIControl *obj)
{
    if (!obj->IsShown())
        return;

    DrawEvent evt;
    obj->GetEventManager().ProcessEvent<DrawEvent>(evt);
    for (ArxObject *obj : const_cast<ArxObjectList&>(obj->GetChildren()))
    {
        UIControl *uiControl = dynamic_cast<UIControl*>(obj);
        if (uiControl)
            DrawNowInternal(uiControl);
    }
}

void ArxWindow::DrawNow()
{
    if (!IsShown())
        return;
    DrawInternal(this);
    SwapBuffersEvent event(m_win.get());
    GetEventManager().ProcessEvent<SwapBuffersEvent>(event);
}

bool ArxWindow::SetIcon(std::optional<std::reference_wrapper<const Image>> img)
{
    if (!img.has_value())
    {
        glfwSetWindowIcon(m_win.get(), 1, nullptr);
        return true;
    }
    
    if (img->get().GetColorChannels() != 4) //accept only RGBA
        return false;
    
    GLFWimage glfwImg;
    glfwImg.height = static_cast<int>(img->get().GetSize().height);
    glfwImg.width = static_cast<int>(img->get().GetSize().width);
    
    //glfw will not modify buffer so don't worry
    glfwImg.pixels = const_cast<unsigned char*>(img->get().GetData().data()); 
    glfwSetWindowIcon(m_win.get(), 1, &glfwImg);
    
    return true;
}

bool ArxWindow::IsEnabledClipToBounds() const
{
    return false;
}

bool ArxWindow::SetWindowAspectRatio(int numer, int denom)
{
    if (numer != constants::IGNORE_VALUE && denom != constants::IGNORE_VALUE && (numer <= 0 || denom <= 0))
        return false;

    glfwSetWindowAspectRatio(m_win.get(), numer, denom);
    return true;
}

void ArxWindow::OnShow(ShowEvent &e)
{
    if (e.WillBeShown())
        glfwShowWindow(m_win.get());
    else
        glfwHideWindow(m_win.get());

    e.Skip();
}

void ArxWindow::EnableVSync(bool enable)
{
    SetAsCurrentContext();
    //wait for monitor refresh
    glfwSwapInterval(enable);
    m_vsyncEnabled = enable;
}



ARX_NAMESPACE_END
