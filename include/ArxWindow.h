#pragma once
#include "ArxDefines.h"
#include "UIObject.h"
#include <glm/mat4x4.hpp>
#include <string_view>

struct GLFWwindow;

ARX_NAMESPACE_BEGIN


class ArxWindow : public UIObject
{
public:
    enum ARX_EXPORTS WindowAttributes : int
    {
        RESIZABLE = 0x01, //window becomes resizable if set
        DECORATED = 0x02, //window will have window decorations if set
        DISPLAY_ON_TOP = 0x04 //display window on top of all window
    };
    
    ArxWindow(std::string_view title, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION, int attributes = WindowAttributes::RESIZABLE | WindowAttributes::DECORATED); //, bool isFullScreen= false);
    
    /**
     * @brief Shows/Hides window
     * @param[in] visible Show/Hide
     */
    void Show(bool visible = true) override;
    void SetWindowAttributes(int attributes);
    
    std::string_view GetTitle();
    void SetTitle(std::string_view title);

    void SetSize(Size s) override;
    void SetPosition(Position pos) override;
    void Reparent(ArxObject *parent) override;

    void SetAsCurrentContext();

    /*virtual void SetFixedViewport(float width, float height);
    virtual void RemoveFixedViewport();*/

    //virtual const glm::mat4 &GetViewport();
    virtual ~ArxWindow();
private:
    std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_win;

    int m_attributes;
    std::string m_title;
    //glm::mat4 m_viewport;
    bool m_useFixedViewport;
};

ARX_NAMESPACE_END
