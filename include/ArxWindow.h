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

    ArxWindow(std::string_view title, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED );
    
    Size GetRectSize() override;

    void SetMinSize(const Size &s) override;
    void SetMaxSize(const Size &s) override;
    void SetSize(const Size &s) override;
    
    void SetPosition(const Position &pos) override;

    void Show(bool visible = true) override;
    void Draw() override;
    void SetFocus(bool focus = true) override;
    void SetWindowAttributes(int attributes);

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


ARX_NAMESPACE_END
