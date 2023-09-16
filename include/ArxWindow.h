#pragma once
#include <memory>
#include <functional>
#include "UIObject.h"

#include <iostream>
#include <vector>
struct GLFWwindow; 

ARX_NAMESPACE_BEGIN

class ARX_EXPORTS ArxWindow : public UIObject
{
public:
    enum ARX_EXPORTS WindowAttributes : int
    {
        RESIZABLE = 0x01,
        DECORATED = 0x02,
        DISPLAY_ON_TOP = 0x04
    };

    ArxWindow(UIObject *parent, std::string_view title, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED );
    
    Size GetRectSize() override;

    void SetMinSize(const Size &s) override;
    void SetMaxSize(const Size &s) override;
    void SetSize(const Size &s) override;
    
    void SetPosition(const Position &pos) override;

    void Show(bool visible = true) override;
    void Draw() override;
    void SetFocus(bool focus = true) override;
    void SetWindowAttributes(int attributes);
    ~ArxWindow() override;

    std::string_view GetTitle();
    void SetTitle(std::string_view title);
    void Close();

private:
    void InitGlfwCallbacks();
    ::GLFWwindow *GetGLFWwindow();
    void SetGLFWwindow(::GLFWwindow *window);
    static void close_callback(GLFWwindow *win);
    static void focus_callback(GLFWwindow *win, int focused);
    static void size_callback(GLFWwindow *win, int width, int height);
    static void position_callback(GLFWwindow *win, int x, int y);
    std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>> m_win;
    std::string m_title;
};

class TopLevelWindowManager
{

public:
    static TopLevelWindowManager &GetInstance()
    {
        static TopLevelWindowManager manager;
        return manager;
    }

    ArxWindow *GetCurrentTopLevelWindow()
    {
        return m_currentWindow;
    }

    void SetCurrentTopLevelWindow(ArxWindow *win)
    {
        (void)FindWindow(win); 
        m_currentWindow = win;
    }

    void AddTopLevelWindow(ArxWindow *win)
    {
        if (m_currentWindow == nullptr)
            m_currentWindow = win;

        m_topLevelWindows.push_back(win);
    }

    void RemoveCurrentTopLevelWindow()
    {
        RemoveTopLevelWindow(m_currentWindow);
    }

    void RemoveTopLevelWindow(ArxWindow *win)
    {
        auto it = FindWindow(win);
        m_topLevelWindows.erase(it);
        if (m_currentWindow == win)
            m_currentWindow = m_topLevelWindows.empty() ? nullptr : m_topLevelWindows.back();
    }

    void DestroyCurrentTopLevelWindow()
    {
        delete m_currentWindow;
        RemoveTopLevelWindow(m_currentWindow);
    }

    bool HasTopLevelWindows()
    {
        return !m_topLevelWindows.empty();
    }
private:
    std::vector<ArxWindow*> m_topLevelWindows;
    using topLevelWindowIterator = decltype(m_topLevelWindows)::iterator;
    decltype(m_topLevelWindows)::iterator FindWindow(ArxWindow *win)
    {
        auto it = std::find_if(m_topLevelWindows.begin(), m_topLevelWindows.end(), [win](ArxWindow* w){ return win == w; });
        assert(it != m_topLevelWindows.end());
        return it;
    }    
    ArxWindow *m_currentWindow;
};

ARX_NAMESPACE_END
