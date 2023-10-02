#pragma once
#include "color.h"
#include "Size.h"
#include "Position.h"
#include <vector>
#include <type_traits>
#include "EventHandler.h"

ARX_NAMESPACE_BEGIN

/**
 * @class SizeEvent
 * @brief event that is raised when object is resized using SetSize
 */
class SizeEvent : public Event
{

};

/**
 * @class DrawEvent
 * @brief placeholder for future drawing rework
 */
class DrawEvent : public Event
{

};

/**
 * @class CloseEvent
 * @brief Event that is raised on Destruction
 */
class CloseEvent : public Event
{

};

/**
 * @class UIObject
 * @brief Base class for GUI objects
 */
class ARX_EXPORTS UIObject : public IEventHandlable 
{
public:
    UIObject(UIObject *parent, Size size = defaults::DEFAULT_SIZE, Position position = defaults::DEFAULT_POSITION);
    virtual ~UIObject();
private:

    /** 
     * @brief Sets new minSize parameter if newParam < maxSizeParam and newParam > rectParam
     */
    int GetNewMinSizeParameter(int newParam, int maxSizeParam, int rectParam);
    
    /** 
     * @brief Sets new maxSize parameter if newParam > minSizeParam and newParam > rectParam
     */
    int GetNewMaxSizeParameter(int newParam, int minSizeParam, int rectParam);

    /** 
     * @brief Sets new size parameter if newParam > minSizeParam and newParam < maxSizeParam and newParam > rectParam
     */
    int GetNewSizeParameter(int newParam, int maxSizeParam, int minSizeParam, int rectParam);
public:
    virtual Size GetRectSize() = 0;
    /**
     * @brief Sets minimal size
     * @description Sets new minimal size limited to
     * newMinSize < maxSize and newMinSize > rectSize
     * Use defaults::IGNORE_SIZE or IGNORE size parameter 
     * if any size is ok.
     * @param[in] s new minimal size 
     */
    virtual void SetMinSize(const Size &s);

    /**
     * @brief Gets minimal size
     * @description Default minimal size is equal to IGNORE_SIZE (any size is ok)
     * @result Minimal size
     */
    Size GetMinSize() const noexcept;

    /**
     * @brief Sets maximum size
     * @description Sets new maximum size limited to
     * newMaxSize > minSize and newMaxSize > rectSize
     * Use defaults::IGNORE_SIZE or IGNORE size parameter 
     * if any size is ok.
     * @param[in] s new maximum size 
     */
    virtual void SetMaxSize(const Size &s);

    /**
     * @brief Gets maximum size
     * @description Default maximum size is equal to IGNORE_SIZE (any size is ok)
     * @result Maximum size
     */
    Size GetMaxSize() const noexcept;

    /**
     * @brief Sets size
     * @description Sets new size limited to
     * newSize > minSize and newSize < maxSize and newSize > rectSize
     * @param[i] s new size
     */
    virtual void SetSize(const Size &s);

    /**
     * @brief Gets size
     * @result size
     */
    Size GetSize() const noexcept;
    
    /**
     * @brief Sets background color
     * @param[i] c new color
     */
    virtual void SetBackgroundColor(Color c);
    
    /**
     * @brief Gets background color
     * @result background color
     */
    Color GetBackgroundColor() const noexcept;

    /**
     * @brief Sets position
     * @param[i] pos Position
     */
    virtual void SetPosition(const Position &pos);

    /**
     * @brief Gets position
     * @result position
     */
    Position GetPosition() const noexcept;
    
    /**
     * @brief Shows/Hides element
     * @description This function shows element and it's up to developer
     * to decide how this function should be implemented
     * @param[in] visible Show or hide
     */
    virtual void Show(bool visible = true) = 0;
    virtual void Hide();

    /**
     * @brief Draws element and it's children
     */
    virtual void Draw();

    /**
     * @brief Sets element focus
     * @description This function sets/unsets focus on element and it's up to developer
     * to decide how this function should be implemented
     * @param[in] focus Focus or lose focus
     */
    virtual void SetFocus(bool focus = true) = 0;
    
    /**
     * @brief Gets parent object 
     * @result UIObject parent
     */
    UIObject *GetParent() noexcept;

    /**
     * @brief Gets object children
     * @result vector of UIObjects
     */
    std::vector<UIObject*> &GetChildren() noexcept;

     /**
     * @brief Sets parent
     * @description Removes child (if not null) from current parent 
     * and adds child to other parent (if not null)
     */
    void SetParent(UIObject *parent);

    /**
     * @brief Removes child if this object is parent
     * @param[in] child UIObject child
     */
    void RemoveChild(UIObject *child);
    
    /**
     * @brief Destroys function in preferable way
     * @description Use this function instead of delete keyword to avoid dangling events.
     * Raises CloseEvent to this object and children
     */
    void Destroy();
protected:
    EventHandler &GetEventHandler() noexcept override;
    const EventHandler &GetEventHandler() const noexcept override;
private:
    UIObject *m_parent;
    Size m_minSize;
    Size m_maxSize;
    Size m_size;
    Position m_position;
    Color m_color;
    std::vector<UIObject*> m_children;
    EventHandler m_eventHandler;
    bool m_destroyCalled;

    /*event part*/
};

ARX_NAMESPACE_END
