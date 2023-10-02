#pragma once
#include <memory>
#include <functional>
#include "UIObject.h"

#include <iostream>
#include <vector>
struct GLFWwindow; 

ARX_NAMESPACE_BEGIN

/**
 * @class ArxWindow
 * @brief Class that represents window
 */
class ARX_EXPORTS ArxWindow : public UIObject
{
public:
    enum ARX_EXPORTS WindowAttributes : int
    {
        RESIZABLE = 0x01, //window becomes resizable if set
        DECORATED = 0x02, //window will have window decorations if set
        DISPLAY_ON_TOP = 0x04 //display window on top of all window
    };

    ArxWindow(std::string_view title, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED );
    
    /**
     * @brief Returns rect size that is equal to (1, 1)
     */
    Size GetRectSize() override;

    void SetMinSize(const Size &s) override;
    void SetMaxSize(const Size &s) override;
    void SetSize(const Size &s) override;
    
    void SetPosition(const Position &pos) override;

    /**
     * @brief Shows/Hides window
     * @param[in] visible Show/Hide
     */
    void Show(bool visible = true) override;
    
    /**
     * @brief Draws window and its children
     */
    void Draw() override;

    /**
     * @brief Focus/Unfocus window to switch context to other window
     */
    void SetFocus(bool focus = true) override;

    /**
     * @brief Sets window attributes
     * Available Attributes:
     * -# RESIZABLE - window becomes resizable if set
     * -# DECORATED - window will have window decorations if set
     * -# DISPLAY_ON_TOP - display window on top of all window
     */
    void SetWindowAttributes(int attributes);
    
    /**
     * @brief Gets window title
     */
    std::string_view GetTitle();

    /**
     * @brief Sets window title shown on top
     */
    void SetTitle(std::string_view title);

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
