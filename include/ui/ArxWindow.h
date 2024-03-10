#ifndef ARX_ARXWINDOW_H
#define ARX_ARXWINDOW_H

#include "ArxDefines.h"
#include "UIControl.h"
#include "Viewport.h"
#include <string_view>

struct GLFWwindow;

ARX_NAMESPACE_BEGIN

class UICache;
class Image;




class ArxWindow : public UIControl
{
public:
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
    
    ArxWindow(std::string_view title, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED); //, bool isFullScreen= false);
    
    
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
    Position GetRealPosition();

    void Reparent(ArxObject *parent) override;
    int GetAttributes() const;
    virtual WindowBorders GetWindowBorders() const;

    ArxWindow *GetWindow() override;
    const ArxWindow *GetWindow() const override;
    void SetAsCurrentContext() const;

    virtual void SetFixedViewport(float width, float height);
    virtual void RemoveFixedViewport();

    virtual const Viewport &GetViewport();
    virtual ~ArxWindow();

    UICache *GetUICache();
    
    //accept only RGBA 8byte per channel images
    //nullopt == unset icon
    bool SetIcon(std::optional<std::reference_wrapper<const Image>> img);
    
    /* schedules draw event */
    void Draw();

    /* redraw now */
    void DrawNow();

    bool IsEnabledClipToBounds() const override;

    //To disable the aspect ratio limit for a window, set both terms to defaults::IGNORE
    bool SetWindowAspectRatio(int numer, int denom);
    ArxWindow *Clone() override;

    void EnableVSync(bool enable);
private:
    ArxWindow *AllocClone() override;
    void OnShow(ShowEvent &e);
    void OnDraw(DrawEvent &e) override;
    void RecalculateSizes(Size s);
    void RegisterWindowFromWindowList();
    void UnregisterWindowFromWindowList();

    static void PositionCallback(GLFWwindow *win, int x, int y);
    static void CloseCallback(GLFWwindow *win);
    static void RefreshCallback(GLFWwindow *win);
    static void InputCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void SetGlfwCallbacks(GLFWwindow *win);

    static void DrawInternal(UIControl *obj);
    static void DrawNowInternal(UIControl *obj);

private:
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_win;
    Size m_clientSize;
    int m_attributes;
    std::string m_title;
    Viewport m_viewport;
    bool m_useFixedViewport;
    std::unique_ptr<UICache> m_uiCache;
    bool m_vsyncEnabled;
};

ARX_NAMESPACE_END

#endif
