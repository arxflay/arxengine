#ifndef ARX_UIOBJECT_H
#define ARX_UIOBJECT_H
#include "ArxDefines.h"
#include "ArxObject.h"

ARX_NAMESPACE_BEGIN

class UIControl;
class ArxWindow;

/*
 * @brief UIObject class for graphic related objects that depends on UIControl
 */
class ARX_EXPORTS UIObject : public ArxObject
{
public:
    //UIControl parent can't be nullptr, otherwise exception will be thrown
    UIObject(UIControl *parent);
    
    //Reparenting is allowed only if OwnerWindow is not changed otherwise expection will be thrown
    void Reparent(ArxObject *parent) override;

    UIControl *GetOwnerUIControl();
    ArxWindow *GetWindow();

    const UIControl *GetOwnerUIControl() const;
    const ArxWindow *GetWindow() const;
};

ARX_NAMESPACE_END

#endif
