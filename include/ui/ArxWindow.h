#ifndef ARX_ARXWINDOW_H
#define ARX_ARXWINDOW_H

#include "ArxDefines.h"
#include "UIControl.h"
#include "Viewport.h"
#include <string_view>
#include "ui/MouseEvent.h"

struct GLFWwindow;

ARX_NAMESPACE_BEGIN

class UICache;
class Image;
class ShowCursorEvent;

class ARX_EXPORTS ArxWindow : public UIControl
{
public:
    friend class ShowCursorEvent;
    enum ARX_EXPORTS WindowAttributes : int
    {
        RESIZABLE = 0x01, //window becomes resizable if set
        DECORATED = 0x02, //window will have window decorations if set
        DISPLAY_ON_TOP = 0x04 //display window on top of all window
    };

    struct WindowBorders
    {
        int top;
        int left;
        int right;
        int bottom;
    };
    
    ArxWindow(std::string_view title, Size size = constants::DEFAULT_SIZE, Position position = constants::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED); //, bool isFullScreen= false);
    
    
    void SetWindowAttributes(int attributes);
    
    std::string_view GetTitle();
    void SetTitle(std::string_view title);

    //Sets ClientSize and calculates size with win borders 
    void SetSize(Size s) override;

    //returns size without window borders
    Size GetClientSize() const override;

    //position is relative to main monitor
    void SetPosition(Position pos) override;

    //non relative window position (multimonitor position)
    Position GetRealPosition() const;

    void Reparent(ArxObject *parent) override;
    int GetAttributes() const;
    virtual WindowBorders GetWindowBorders() const;

    ArxWindow *GetWindow() override;
    const ArxWindow *GetWindow() const override;
    void SetAsCurrentContext() const;

    virtual void SetFixedViewport(float width, float height);
    virtual void RemoveFixedViewport();

    virtual const Viewport &GetViewport() const;
    virtual ~ArxWindow();

    UICache *GetUICache();

    void ShowCursor(bool show);
    bool IsCursorVisible() const;
    
    //accept only RGBA 8byte per channel images
    //nullopt == unset icon
    bool SetIcon(std::optional<std::reference_wrapper<const Image>> img);
    
    /* schedules draw event */
    void Draw();

    /* redraw now */
    void DrawNow();

    bool IsEnabledClipToBounds() const override;

    //To disable the aspect ratio limit for a window, set both terms to constants::IGNORE_VALUE
    bool SetWindowAspectRatio(int numer, int denom);
    ArxWindow *Clone() override;

    void EnableVSync(bool enable);

    Position GetCursorPosition();
private:
    ArxWindow *AllocClone() override;
    void OnShow(ShowEvent &e);
    void OnDraw(DrawEvent &e) override;
    void RecalculateSizes(Size s);
    void RegisterWindowFromWindowList();
    void UnregisterWindowFromWindowList();

    void SendMouseEnterExitEvents(UIControl *ctrl, Position pos);
    void SendForcefullyMouseExitEvents();


    static void PositionCallback(GLFWwindow *win, int x, int y);
    static void CloseCallback(GLFWwindow *win);
    static void RefreshCallback(GLFWwindow *win);
    static void KeyboardInputCallback(GLFWwindow *win, int key, int scancode, int action, int mods);
    static void MouseInputCallback(GLFWwindow *win, int button, int action, int mods);
    static void MouseCallback(GLFWwindow *win, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow *win, double xpos, double ypos);
    static void MouseEnterExitCallback(GLFWwindow *win, int enter);

    static void MouseScrollCallback(GLFWwindow *win, int button, int action, int mods);

    static void SetGlfwCallbacks(GLFWwindow *win);

    static void DrawInternal(UIControl *obj);
    static void DrawNowInternal(UIControl *obj);

    struct PressedMouseButtonRecord
    {
        PressedMouseButtonRecord(UIControl *controlIn, MouseButtonEvent::ButtonType buttonTypeIn)
            : control(controlIn), buttonType(buttonTypeIn)
        {}
        UIControl *control;
        MouseButtonEvent::ButtonType buttonType;
        bool operator<(const PressedMouseButtonRecord &rec) const { return buttonType < rec.buttonType; }
        bool operator>(const PressedMouseButtonRecord &rec) const { return buttonType > rec.buttonType; }
    };

private:
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_win;
    Size m_clientSize;
    int m_attributes;
    std::string m_title;
    Viewport m_viewport;
    bool m_useFixedViewport;
    std::unique_ptr<UICache> m_uiCache;
    bool m_vsyncEnabled;
    bool m_showCursor;
    std::set<PressedMouseButtonRecord> m_pressedMouseButtons;
    std::set<UIControl*>  m_mouseEnteredControls;
};

ARX_NAMESPACE_END

#endif
